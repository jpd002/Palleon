#include <cassert>
#include "palleon/EmbedRemoteCall.h"
#include "xml/Writer.h"
#include "xml/Utils.h"
#include "xml/Parser.h"
#include "MemStream.h"
#include "PtrStream.h"

using namespace Palleon;

CEmbedRemoteCall::CEmbedRemoteCall()
{

}

CEmbedRemoteCall::CEmbedRemoteCall(const std::string& rpcString)
{
	ReadCall(rpcString);
}

CEmbedRemoteCall::~CEmbedRemoteCall()
{

}

std::string CEmbedRemoteCall::GetMethod() const
{
	return m_method;
}

void CEmbedRemoteCall::SetMethod(const std::string& method)
{
	m_method = method;
}

std::string CEmbedRemoteCall::GetParam(const std::string& name) const
{
	auto paramIterator = m_params.find(name);
	assert(paramIterator != std::end(m_params));
	return paramIterator->second;
}

void CEmbedRemoteCall::SetParam(const std::string& name, const std::string& value)
{
	assert(m_params.find(name) == std::end(m_params));
	m_params[name] = value;
}

std::string CEmbedRemoteCall::ToString() const
{
	auto rpcParamsNode = new Framework::Xml::CNode("Params", true);
	for(const auto& param : m_params)
	{
		auto rpcParamNode = new Framework::Xml::CNode("Param", true);
		rpcParamNode->InsertAttribute(Framework::Xml::CreateAttributeStringValue("Name", param.first.c_str()));
		rpcParamNode->InsertAttribute(Framework::Xml::CreateAttributeStringValue("Value", param.second.c_str()));
		rpcParamsNode->InsertNode(rpcParamNode);
	}

	auto rpcNode = std::make_unique<Framework::Xml::CNode>("Call", true);
	rpcNode->InsertAttribute(Framework::Xml::CreateAttributeStringValue("Method", m_method.c_str()));
	rpcNode->InsertNode(rpcParamsNode);

	Framework::CMemStream rpcNodeStream;
	Framework::Xml::CWriter::WriteDocument(rpcNodeStream, rpcNode.get());

	return std::string(rpcNodeStream.GetBuffer(), rpcNodeStream.GetBuffer() + rpcNodeStream.GetSize());
}

void CEmbedRemoteCall::ReadCall(const std::string& rpcString)
{
	Framework::CPtrStream rpcNodeStream(rpcString.c_str(), rpcString.length());
	auto rpcRootNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(rpcNodeStream));

	auto rpcNode = rpcRootNode->Select("Call");
	m_method = Framework::Xml::GetAttributeStringValue(rpcNode, "Method");

	auto rpcParamNodes = rpcNode->SelectNodes("Params/Param");
	for(const auto& rpcParamNode : rpcParamNodes)
	{
		auto paramName = Framework::Xml::GetAttributeStringValue(rpcParamNode, "Name");
		auto paramValue = Framework::Xml::GetAttributeStringValue(rpcParamNode, "Value");
		SetParam(paramName, paramValue);
	}
}
