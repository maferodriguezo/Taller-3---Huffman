#ifndef __BinTree__h__
#define __BinTree__h__

/**
 */
template< class _T >
class BinTree
{
public:
  using Self = BinTree;

public:
  BinTree( const _T& v )
    : m_V( v )
    {
    }
  virtual ~BinTree( )
    {
      if( this->m_L != nullptr )
        delete this->m_L;
      if( this->m_R != nullptr )
        delete this->m_R;
    }

  void set_left( Self* l )
    {
      if( this->m_L != nullptr )
        delete this->m_L;
      this->m_L = l;
    }

  void set_right( Self* l )
    {
      if( this->m_R != nullptr )
        delete this->m_R;
      this->m_R = l;
    }

  long long height( ) const
    {
      long long l = ( this->m_L != nullptr )? this->m_L->height( ): 0;
      long long r = ( this->m_R != nullptr )? this->m_R->height( ): 0;
      return( 1 + ( ( r < l )? l: r ) );
    }
  long long weight( ) const
    {
      long long l = ( this->m_L != nullptr )? this->m_L->weight( ): 0;
      long long r = ( this->m_R != nullptr )? this->m_R->weight( ): 0;
      return( 1 + l + r );
    }

  void print_preorder( std::ostream& o, const std::string& indent = "" ) const
    {
      o << indent << this->m_V << std::endl;
      if( this->m_L != nullptr )
        this->m_L->print_preorder( o, indent + " " );
      else
        o << indent << " +" << std::endl;
      if( this->m_R != nullptr )
        this->m_R->print_preorder( o, indent + " " );
      else
        o << indent << " +" << std::endl;
    }

protected:
  _T    m_V;
  Self* m_L { nullptr };
  Self* m_R { nullptr };
};

#endif // __BinTree__h__

// eof - BinTree.h
