#include <exception>

class creaturesException : public std::exception {
protected:
	const char *r;

public:
	creaturesException(const char *s) throw() { r = s; }
	const char* what() throw() { return r; }
};

class genomeException : public creaturesException {
public:
	genomeException(const char *s) throw() : creaturesException(s) { }
};
