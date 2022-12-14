#include "../../include/MiniParser.hpp"

/*
Version History:
	- 2006-04-08
		initial release
	- 2006-12-09:
		added math functions (sin, cos, log, ln, exp, sqrt) and constants (PI)
		improved error handling (removed an infinite loop)
	- 2007-04-23:
		made the grammar left associative to match natural arithmetic and C
*/

#include <iostream>
#include <sstream>
#include <map>
#include <cmath>
#include <cassert>
const float PI = 3.1415;
namespace
{
    typedef MiniParserNode Node ;

    // This is it!
    Node* Parse( const std::string& str ) ;
}

class MiniParserNode
{
public:
    typedef MiniParser::IDMap IDMap ;
    typedef MiniParser::real real ;

    static MiniParserNode* FromString( const std::string& str )
    {
        return Parse( str ) ;
    }

    virtual ~MiniParserNode() {}

    virtual real eval( const IDMap& ids ) const = 0 ;
};

namespace
{
typedef MiniParserNode::real real ;

class NodeConstantValue : public Node
{
public:
    NodeConstantValue( real val ) : mVal( val ) {}

    real eval( const IDMap& ids ) const
    {
        return mVal ;
    }

private:
    real mVal ;
};

class NodeVariableValue : public Node
{
public:
    NodeVariableValue( const std::string& var ) : mVar( var ) {}

    real eval( const IDMap& ids ) const
    {
        IDMap::const_iterator var = ids.find( mVar ) ;
        if( ids.end() == var )
        {
            std::cerr << "unknown variable: " << mVar << std::endl ;
            return 0. ;
        }
        return var->second ;
    }

private:
    const std::string mVar ;
};

class NodePlus : public Node
{
public:
    NodePlus( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodePlus() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) + mRHS->eval( ids ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeMinus : public Node
{
public:
    NodeMinus( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeMinus() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) - mRHS->eval( ids ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeTimes : public Node
{
public:
    NodeTimes( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeTimes() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) * mRHS->eval( ids ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeDividedBy : public Node
{
public:
    NodeDividedBy( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeDividedBy() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) / mRHS->eval( ids ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeModulus : public Node
{
public:
    NodeModulus( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeModulus() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return fmod( mLHS->eval( ids ), mRHS->eval( ids ) ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodePow : public Node
{
public:
    NodePow( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodePow() { delete mLHS ; delete mRHS ; }

    real eval( const IDMap& ids ) const
    {
        return pow( mLHS->eval( ids ), mRHS->eval( ids ) ) ;
    }

private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeNegate : public Node
{
public:
    NodeNegate( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeNegate() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return -mNode->eval( ids ) ;
    }

private:
    Node* mNode ;
};
class NodeSine : public Node
{
public:
    NodeSine( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeSine() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return sin( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};
class NodeCosine : public Node
{
public:
    NodeCosine( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeCosine() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return cos( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};
class NodeLog : public Node
{
public:
    NodeLog( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeLog() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return log10( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};
class NodeLN : public Node
{
public:
    NodeLN( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeLN() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return log( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};
class NodeExp : public Node
{
public:
    NodeExp( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeExp() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return exp( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};
class NodeSqrt : public Node
{
public:
    NodeSqrt( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeSqrt() { delete mNode ; }

    real eval( const IDMap& ids ) const
    {
        return sqrt( mNode->eval( ids ) ) ;
    }

private:
    Node* mNode ;
};

class MiniTokenizer
{
public:
    MiniTokenizer( std::istream& in ) : mIn( in ) { mIn >> std::ws ; advance() ; }

    std::string eat() { std::string result = mPeek ; advance() ; return result ; }
    const std::string& peek() { return mPeek ; }
    bool eof() { return mIn.eof() ; }

private:
    void advance()
    {
        if( eof() )
            mPeek = "" ;
        else
            mIn >> mPeek >> std::ws ;
    }

    std::istream& mIn ;
    std::string mPeek ;
};

void find_replace( std::string& in_this_string, const std::string& find, const std::string& replace )
{
    std::string::size_type pos = 0 ;
    while( std::string::npos != (pos = in_this_string.find( find, pos )) )
    {
        in_this_string.replace( pos, find.length(), replace ) ;
        pos += replace.length() ;
    }
}

Node* ParseValueNode( MiniTokenizer& mt ) ;

Node* Parse( const std::string& str )
{
    std::string input = str ;
    // Our simple tokenizer needs whitespace between all tokens.
    find_replace( input, "(", " ( " ) ;
    find_replace( input, ")", " ) " ) ;
    find_replace( input, "+", " + " ) ;
    find_replace( input, "-", " - " ) ;
    find_replace( input, "*", " * " ) ;
    find_replace( input, "/", " / " ) ;
    find_replace( input, "%", " % " ) ;
    find_replace( input, "^", " ^ " ) ;
    // The exponentiation operator "**" gets turned into "*  *" by the above
    // find/replaces.  Turn it back into "**".
    find_replace( input, "*  *", "**" ) ;

    /*
    // Operator precedence can be implemented with some parentheses.
    // See: http://en.wikipedia.org/wiki/Operator-precedence_parser#Alternatives_to_Dijkstra.27s_Algorithm
    // First in precedence is exponentiation.
    find_replace( input, " ^ ", " ) ^ ( " );
    find_replace( input, " ** ", " ) ** ( " );
    // Then multiplication and division and modulus.
    find_replace( input, " * ", " ) ) ) * ( ( ( " );
    find_replace( input, " / ", " ) ) ) / ( ( ( " );
    find_replace( input, " % ", " ) ) ) % ( ( ( " );
    // Finally addition and subtraction.
    find_replace( input, " + ", " ) ) ) ) + ( ( ( ( " );
    // NOTE: Whoa, subtraction is complicated by its dual-use as unary minus, which has
    //       precedence after exponentiation but before multiplication/division/modulus.
    // find_replace( input, " - ", " ) ) ) ) - ( ( ( ( " );
    {
        std::string& in_this_string = input;
        const std::string find( " - " );
        const std::string replace1( " ) ) ) ) - ( ( ( ( " );
        const std::string replace2( " 0 ) ) - ( ( " );
        // const std::string replace2( " - " );

        std::string::size_type pos = 0 ;
        while( std::string::npos != (pos = in_this_string.find( find, pos )) )
        {
            bool preceded_by_open_paren = true;
            for( int i = pos-1; i >= 0; --i )
            {
                if( in_this_string[i] != ' ' && in_this_string[i] != '\r' && in_this_string[i] != '\n' && in_this_string[i] != '\t' && in_this_string[i] != '(' )
                {
                    preceded_by_open_paren = false;
                    break;
                }
            }

            if( preceded_by_open_paren )
            {
                in_this_string.replace( pos, find.length(), replace2 ) ;
                pos += replace2.length() ;
            }
            else
            {
                in_this_string.replace( pos, find.length(), replace1 ) ;
                pos += replace1.length() ;
            }
        }
    }
    // We need to parse an expression surrounded by four parentheses.
    std::istringstream istr( "( ( ( ( ( " + input + " ) ) ) ) )" ) ;

    std::cout << istr.str() << std::endl;
    */

    std::istringstream istr( "( " + input + " )" ) ;
    MiniTokenizer tt( istr ) ;
    return ParseValueNode( tt ) ;
}

Node* ParseExpressionNode( MiniTokenizer& mt ) ;
Node* ParseValueNode( MiniTokenizer& mt )
{
    std::string t = mt.eat() ;
    if( t.size() == 0 )
    {
        std::cerr << "expression cut short" << std::endl ;
        return new NodeConstantValue( 0. ) ;
    }

    assert( t.size() > 0 ) ;
    if( t == "(" )
    {
        Node* result = ParseExpressionNode( mt ) ;
        std::string rp = mt.eat() ;
        if( ")" != rp )
        {
            std::cerr << "expression error: expected \")\"" << std::endl ;
            delete result ;
            return new NodeConstantValue( 0. ) ;
        }
        return result ;
    }
    else if( t == "-" )
    {
        return new NodeNegate( ParseValueNode( mt ) ) ;
    }
    else if( t[0] == '.' || (t[0] >= '0' && t[0] <= '9') )
    {
        std::istringstream ttof( t ) ;
        real val ;
        ttof >> val ;
        if( !ttof )
        {
            std::cerr << "expecting a real number but received: " << t << std::endl ;
            return new NodeConstantValue( 0. ) ;
        }
        else
        {
            return new NodeConstantValue( val ) ;
        }
    }
    else if( t == "sin" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeSine( ParseValueNode( mt ) ) ;
    }
    else if( t == "cos" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeCosine( ParseValueNode( mt ) ) ;
    }
    else if( t == "log" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeLog( ParseValueNode( mt ) ) ;
    }
    else if( t == "ln" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeLN( ParseValueNode( mt ) ) ;
    }
    else if( t == "exp" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeExp( ParseValueNode( mt ) ) ;
    }
    else if( t == "sqrt" )
    {
        if( mt.peek() != "(" )
        {
            std::cerr << "built-in function " << t << " must be followed by parentheses." << std::endl ;
            return new NodePlus( new NodeConstantValue( 0. ), ParseExpressionNode( mt ) ) ;
        }
        return new NodeSqrt( ParseValueNode( mt ) ) ;
    }
    else if( t == "PI" )
    {
        return new NodeConstantValue( PI ) ;
    }
    else
    {
        return new NodeVariableValue( t ) ;
    }
}

Node* MakeOperationNode( const std::string& op, Node* lhs, Node* rhs )
{
    if( op == "+" )
    {
        return new NodePlus( lhs, rhs ) ;
    }
    else if( op == "-" )
    {
        return new NodeMinus( lhs, rhs ) ;
    }
    else if( op == "*" )
    {
        return new NodeTimes( lhs, rhs ) ;
    }
    else if( op == "/" )
    {
        return new NodeDividedBy( lhs, rhs ) ;
    }
    else if( op == "%" )
    {
        return new NodeModulus( lhs, rhs ) ;
    }
    else if( op == "**" )
    {
        return new NodePow( lhs, rhs ) ;
    }
    else if( op == "^" )
    {
        return new NodePow( lhs, rhs ) ;
    }
    // error: unknown
    else
    {
        std::cerr << "unknown operation: " << op << std::endl ;
        delete lhs ;
        delete rhs ;
        return new NodeConstantValue( 0. ) ;
    }
}

Node* ParseExpressionNode_RightAssociative( MiniTokenizer& mt )
{
    Node* lhs = ParseValueNode( mt ) ;
    // short circuit if we're a single value
    if( mt.peek() == ")" ) return lhs ;

    std::string op = mt.eat() ;
    // error: prematurely expression end
    if( op.size() == 0 )
    {
        std::cerr << "expression cut short" << std::endl ;
        delete lhs ;
        return new NodeConstantValue( 0. ) ;
    }

    Node* rhs = ParseExpressionNode( mt ) ;

    return MakeOperationNode( op, lhs, rhs );
}
Node* ParseExpressionNode_LeftAssociative( MiniTokenizer& mt )
{
    Node* lhs = ParseValueNode( mt ) ;

    // short circuit when we reach the right parenthesis
    while( mt.peek() != ")" )
    {
        std::string op = mt.eat() ;
        // error: prematurely expression end
        if( op.size() == 0 )
        {
            std::cerr << "expression cut short" << std::endl ;
            delete lhs ;
            return new NodeConstantValue( 0. ) ;
        }

        Node* rhs = ParseValueNode( mt ) ;

        lhs = MakeOperationNode( op, lhs, rhs );
    }

    return lhs ;
}
Node* ParseExpressionNode( MiniTokenizer& mt )
{
    // Most grammars are left associative (e.g. natural arithmetic, C).
    return ParseExpressionNode_LeftAssociative( mt ) ;
}

} // ~unnamed namespace


MiniParser::MiniParser( const std::string& expression )
    : mParseTree( NULL )
{
    mParseTree = MiniParserNode::FromString( expression ) ;
}

MiniParser::~MiniParser()
{
    delete mParseTree ;
}

real
MiniParser::eval( const IDMap& ids )
const
{
    if( !mParseTree )
    {
        std::cerr << "No parse tree!" << std::endl ;
        return 0. ;
    }

    if( ids.count( "PI" ) )
    {
        std::cerr << "Warning: PI is a built-in constant; the value passed in the IDMap will be ignored." << std::endl ;
    }

    return mParseTree->eval( ids ) ;
}
