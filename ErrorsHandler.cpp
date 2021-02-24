#include "ErrorsHandler.h"

void ErrorsHandler::setErrorsHandlerDelegate(std::function<void(void* object)> _delegate)
{
	errorsHandlerDelegate = _delegate;
	errorsHandlerDelegateIsSet = true;
}

void ErrorsHandler::unsetErrorsHandlerDelegate()
{
	errorsHandlerDelegateIsSet = false;
}

void ErrorsHandler::clearErrors()
{
	errors.clear();
}
