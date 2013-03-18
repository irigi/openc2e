/*
 *  AgentRef.h
 *  openc2e
 *
 *  Created by Bryan Donlan on The Apr 11 2005
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#ifndef AGENTREF_H
#define AGENTREF_H 1

#include <cstdlib> // for NULL
#include <iostream>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

class IrigiAgent;

class AgentRef {
	friend class IrigiAgent;
	
protected:
	boost::weak_ptr<IrigiAgent> ref;
	void checkLife() const;

public:
	void dump() const;
	
	AgentRef() { }
	AgentRef(boost::shared_ptr<IrigiAgent> a) { ref = a; }
	AgentRef(boost::weak_ptr<IrigiAgent> a) { ref = a; }
	AgentRef(IrigiAgent *a) { set(a); }
	AgentRef(const AgentRef &r) : ref(r.ref) {}

	void clear() { ref.reset(); }
	
	~AgentRef() { clear(); };

	AgentRef &operator=(const AgentRef &r) { ref = r.ref; return *this; }
	IrigiAgent *operator=(IrigiAgent *a) { set(a); return a; }
	IrigiAgent &operator*() const { checkLife(); return *ref.lock().get(); }
	IrigiAgent *operator->() const { checkLife(); return ref.lock().get(); }
	bool operator!() const { return lock().get() == NULL; }
	/* This next line breaks builds with MSVC, tossing errors about ambiguous operators.
	operator bool() const { return ref; } */
	operator IrigiAgent *() const { return ref.lock().get(); }
	bool operator==(const AgentRef &r) const { return lock() == r.lock(); }
	bool operator==(const IrigiAgent *r) const { return r == lock().get(); }
	bool operator!=(const AgentRef &r) const { return !(*this == r);}
	bool operator!=(const IrigiAgent *r) const { return !(*this == r); }

	void set(IrigiAgent *a);
	void set(const AgentRef &r) { ref = r.ref; }
	void set(const boost::shared_ptr<IrigiAgent> &r) { ref = r; }
	void set(const boost::weak_ptr<IrigiAgent> &r) { ref = r; }

	boost::shared_ptr<IrigiAgent> lock() const;
	IrigiAgent *get() const { return lock().get(); }
};
		

#endif

/* vim: set noet: */
