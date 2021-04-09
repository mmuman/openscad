#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include "Assignment.h"
#include "evaluationsession.h"
#include "function.h"
#include "memory.h"
#include "module.h"
#include "value.h"
#include "valuemap.h"

/**
 * Local handle to a all context objects. This is used to maintain the
 * dynamic scoping stack using object lifetime.
 * The Context objects can hang around for longer, e.g. in case of closures.
 */
template<typename T>
struct ContextHandle
{
    ContextHandle(std::shared_ptr<T>&& sp) : ctx(std::move(sp)) {
		ctx->init();
		ctx->session()->push_context(ctx.get());
    }
    ~ContextHandle() {
		ctx->session()->pop_context();
    }

	ContextHandle(const ContextHandle&) = delete;
	ContextHandle& operator=(const ContextHandle&) = delete;
	ContextHandle(ContextHandle&&) = default;
	ContextHandle& operator=(ContextHandle&&) = delete;
	
	// Valid only if ctx is on the top of the stack.
	ContextHandle& operator=(std::shared_ptr<T>&& sp)
	{
		ctx->session()->pop_context();
		ctx = std::move(sp);
		ctx->session()->push_context(ctx.get());
		return *this;
	}

	const T* operator->() const { return ctx.get(); }
    T* operator->() { return ctx.get(); }

    std::shared_ptr<T> ctx;
};

class EvalContext;

class Context : public std::enable_shared_from_this<Context>
{
protected:
	Context(EvaluationSession* session);
	Context(const std::shared_ptr<Context> parent);

public:
	std::shared_ptr<Context> get_shared_ptr() const { return const_cast<Context*>(this)->shared_from_this(); }

    template<typename C, typename ... T>
    static ContextHandle<C> create(T&& ... t) {
        return ContextHandle<C>{std::shared_ptr<C>(new C(std::forward<T>(t)...))};
    }

	virtual ~Context();
	virtual void init() { }

	const std::shared_ptr<Context> &getParent() const { return this->parent; }

	virtual boost::optional<CallableFunction> lookup_local_function(const std::string &name, const Location &loc) const;
	boost::optional<CallableFunction> lookup_function(const std::string &name, const Location &loc) const;
	virtual boost::optional<InstantiableModule> lookup_local_module(const std::string &name, const Location &loc) const;
	boost::optional<InstantiableModule> lookup_module(const std::string &name, const Location &loc) const;

	void setVariables(const std::shared_ptr<EvalContext> &evalctx, const AssignmentList &parameters, const AssignmentList &optional_parameters={}, bool usermodule=false);

	void set_variable(const std::string &name, Value&& value);

	void apply_variables(const ValueMap& variables);
	void apply_variables(const std::shared_ptr<Context> &other);
	void apply_config_variables(const std::shared_ptr<Context> &other);
	const Value& lookup_variable(const std::string &name, bool silent = false, const Location &loc=Location::NONE) const;
	double lookup_variable_with_default(const std::string &variable, const double &def, const Location &loc=Location::NONE) const;
	const std::string& lookup_variable_with_default(const std::string &variable, const std::string &def, const Location &loc=Location::NONE) const;
	Value lookup_local_config_variable(const std::string &name) const;

	bool has_local_variable(const std::string &name) const;

	EvaluationSession* session() const { return evaluation_session; }
	const std::string &documentRoot() const { return evaluation_session->documentRoot(); }

public:

protected:
	const std::shared_ptr<Context> parent;
	ValueMap variables;
	ValueMap config_variables;

	EvaluationSession* evaluation_session;

public:
#ifdef DEBUG
	virtual std::string dump(const class AbstractModule *mod, const ModuleInstantiation *inst);
#endif
};
