#ifndef __SymmetricSecondRankTensor__h__
#define __SymmetricSecondRankTensor__h__

#include <vector>

/**
 */
template< class _T >
class SymmetricSecondRankTensor
{
public:
  using Self = SymmetricSecondRankTensor;

public:
  SymmetricSecondRankTensor( long long n, const _T& init = _T( ) )
    : m_N( n )
    {
      this->m_V.resize( ( n * ( n + 1 ) ) >> 1, init );
      this->m_V.shrink_to_fit( );
    }

  virtual ~SymmetricSecondRankTensor( ) = default;

  _T& operator()( long long i, long long j )
    {
      return( this->m_V[ this->_i( i, j ) ] );
    }

  const _T& operator()( long long i, long long j ) const
    {
      return( this->m_V[ this->_i( i, j ) ] );
    }

protected:
  inline long long _i( long long i, long long j ) const
    {
      long long k = ( i * this->m_N ) - ( ( i * ( i + 1 ) ) >> 1 ) + j;
      return( ( k < this->m_V.size( ) )? k: 0 );
    }

protected:
  std::vector< _T > m_V;
  long long m_N;
};

#endif // __SymmetricSecondRankTensor__h__
