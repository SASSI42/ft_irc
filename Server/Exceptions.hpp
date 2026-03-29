
#ifndef EXCEPT_H
#define EXCEPT_H

#include <exception>
#include <string>

class BaseException : public std::exception {
	private:
		BaseException();
		std::string message;
	public:
		BaseException(std::string msg);
		virtual void		cleanup();
		virtual const char*	what() const throw() ;
		virtual				~BaseException() throw();
};

class InitException : public BaseException {
	public:
		InitException(std::string msg);
		virtual void cleanup();
};

class RunException : public BaseException {
	public:
		RunException(std::string msg);
		virtual void cleanup(); 
};

#endif