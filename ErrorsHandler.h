#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <functional>
#include <string>
#include <vector>

class ErrorsHandler
{
public:
	void setErrorsHandlerDelegate(std::function<void(void* object)> _delegate);
	void unsetErrorsHandlerDelegate();
	std::vector<std::string> getErrors() const
	{
		return errors;
	}
	void clearErrors();
	void invoke(void* object)
	{
		if (errorsHandlerDelegateIsSet)
			errorsHandlerDelegate(object);
	}
	void pushBackError(const std::string& error)
	{
		errors.push_back(error);
	}
	void pushBackError(std::string&& error)
	{
		errors.emplace_back(std::move(error));
	}
	std::string getLastError() const
	{
		return errors.size() > 0 ? errors[errors.size() - 1] : "";
	}
	size_t errorsCount() const
	{
		return errors.size();
	}

private:
	std::function<void(void*)> errorsHandlerDelegate;
	bool errorsHandlerDelegateIsSet = false;
	std::vector<std::string> errors;
};

#endif // !ERRORHANDLER_H