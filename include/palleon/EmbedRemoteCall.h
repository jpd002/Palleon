#pragma once

#include <map>
#include <string>

namespace Palleon
{
	class CEmbedRemoteCall
	{
	public:
		typedef std::map<std::string, std::string> ParamMap;

							CEmbedRemoteCall();
							CEmbedRemoteCall(const std::string&);
		virtual				~CEmbedRemoteCall();

		std::string			GetMethod() const;
		void				SetMethod(const std::string&);

		std::string			GetParam(const std::string&) const;
		void				SetParam(const std::string&, const std::string&);

		std::string			ToString() const;

	private:
		void				ReadCall(const std::string&);
		
		std::string			m_method;
		ParamMap			m_params;
	};
};
