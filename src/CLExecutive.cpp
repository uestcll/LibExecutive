#include "CLExecutive.h"
#include "CLExecutiveFunctionProvider.h"

CLExecutive::CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider)
{
	if(pExecutiveFunctionProvider == 0)
		throw "In CLExecutive::CLExecutive(), pExecutiveFunctionProvider error";
	
	m_pExecutiveFunctionProvider = pExecutiveFunctionProvider;
}

CLExecutive::~CLExecutive()
{
	delete m_pExecutiveFunctionProvider;
}
