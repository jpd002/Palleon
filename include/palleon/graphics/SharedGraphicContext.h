#pragma once

namespace Palleon
{
	class CSharedGraphicContext
	{
	public:
		virtual			~CSharedGraphicContext() {}

		virtual void	Flush() = 0;
	};

	typedef std::shared_ptr<CSharedGraphicContext> SharedGraphicContextPtr;
};
