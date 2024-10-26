#include <iostream>
#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>

namespace proto = boost::proto;
namespace groklab {
    inline proto::terminal<std::ostream &>::type cout_ = { std::cout };

    // Define a placeholder type
    template<int I>
    struct placeholder
    {};

    template<typename Expr>
    void evaluate(Expr const &expr) {
        proto::default_context ctx;
        proto::eval(expr, ctx);
    }

    template<typename Expr>
    void print_expr_tree(Expr const &expr) {
        proto::display_expr(expr);
    }
}