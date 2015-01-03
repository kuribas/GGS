// $Id: VC_Formula.H 160 2007-06-22 15:21:10Z mburo $
// This is a GGS file, licensed under the GPL

/*
    (c) Igor Durdanovic, igord@research.nj.nec.com
    NEC Research Institute
    4 Independence Way
    Princeton, NJ 08540, USA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//: VC_Formula.H (bof) (c) Igor Durdanovic

#ifndef VC_Formula_H
#define VC_Formula_H

#include "CPP.H"
#include "String.H"
#include <cstring>
#include <iostream>

class Client;
class Request;

// do not close cout in main.C when debugging

namespace VC_Exp
{
  struct Val
  {
    enum TYPE {
      BOOL = 0x01, INT = 0x02, REAL = 0x04, COLOR = 0x08,
      ALL = BOOL | INT | REAL | COLOR,
      NUM = INT  | REAL,
    };

    static ccptr type( TYPE T );
    
    TYPE t;
  
    union {
      bool   b;
      sint4  i;
      real8  r;
      char   c;
    };
  
    Val( bool  V ) : t(BOOL), b(V) {}
    Val( sint4 V ) : t(INT),  i(V) {}
    Val( real8 V ) : t(REAL), r(V) {}
    Val( char  V ) : t(COLOR),c(V) {}

    std::ostream& print( std::ostream& os ) const;
  };

  inline std::ostream& operator<< ( std::ostream& os, const Val& v ) { return v.print( os ); }

  struct Set
  {
    uint4 t;
    explicit Set( uint4 T ) : t(T) {};
    std::ostream& print( std::ostream& os ) const;
  };
  
  inline std::ostream& operator<< ( std::ostream& os, const Set& v ) { return v.print(os); }
  

  struct Exp
  {
    virtual ~Exp() {}
    static Exp* parse( std::ostream& os, String& S );

    virtual std::ostream& print( std::ostream& os ) const = 0;
    
    virtual Val::TYPE type() const = 0;
    virtual Val       eval( const Client* C, const Request& R ) const = 0;
  };


  struct Exp1 : public Exp
  {
    Exp* a1;

    Exp1( Exp* A1 ) : a1(A1) {}
    virtual ~Exp1() { delete a1; }
    std::ostream& print( std::ostream& os ) const
    { return a1->print( os << '(' ) << ')'; }
  };

  struct Exp2 : public Exp
  {
    Exp* a1;
    Exp* a2;

    Exp2( Exp* A1, Exp* A2 ) : a1(A1), a2(A2) {}
    virtual ~Exp2() { delete a1; delete a2; }
    std::ostream& print( std::ostream& os ) const
    { return a2->print( a1->print( os << '(' ) << ',' ) << ')'; }
  };

  struct VExp : public Exp
  {
    Val v;
    VExp( Val V ) : v(V) {}
    Val::TYPE type() const { return v.t; }
    Val eval( const Client* /*C*/, const Request& ) const { return v; }
    std::ostream& print( std::ostream& os ) const { return v.print( os ); }
  };
  
  struct SYNCHRO : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "synchro"; }
};

  struct SAVED : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "saved"; }
  };

  struct STORED : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "stored"; }
  };

  struct RATED : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "rated"; }
  };

  struct RAND : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "rand"; }
  };

  struct KOMI : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "komi"; }
  };

  struct ANTI : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "anti"; }
  };

  struct DISCS : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "discs"; }
  };

  struct SIZE : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "size"; }
  };

  struct MC : public Exp
  {
    Val::TYPE type() const { return Val::COLOR; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mc"; }
  };
  struct MT1 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mt1"; }
  };
  struct MM1 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mm1"; }
  };
  struct ML1 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ml1"; }
  };
  struct MT2 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mt2"; }
  };
  struct MM2 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mm2"; }
  };
  struct MA2 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ma2"; }
  };
  struct MT3 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mt3"; }
  };
  struct MM3 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mm3"; }
  };
  struct MA3 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ma3"; }
  };
  struct MR : public Exp
  {
    Val::TYPE type() const { return Val::REAL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "mr"; }
  };
  struct MD : public Exp1
  {
    MD( Exp* a1 ) : Exp1(a1) {}
    Val::TYPE type() const { return Val::REAL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp1::print(os << "md"); }
  };

  struct OC : public Exp
  {
    Val::TYPE type() const { return Val::COLOR; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "oc"; }
  };
  struct OT1 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ot1"; }
  };
  struct OM1 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "om1"; }
  };
  struct OL1 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ol1"; }
  };
  struct OT2 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ot2"; }
  };
  struct OM2 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "om2"; }
  };
  struct OA2 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "oa2"; }
  };
  struct OT3 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "ot3"; }
  };
  struct OM3 : public Exp
  {
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "om3"; }
  };
  struct OA3 : public Exp
  {
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "oa3"; }
  };
  struct OR : public Exp
  {
    Val::TYPE type() const { return Val::REAL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return os << "or"; }
  };
  struct OD : public Exp1
  {
    OD( Exp* a1 ) : Exp1(a1) {}
    Val::TYPE type() const { return Val::REAL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp1::print( os << "od" ); }
  };
  struct RND : public Exp1
  {
    RND( Exp* a1 ) : Exp1(a1) {}
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp1::print( os << "rnd" ); }
  };
  struct PLAYING : public Exp1
  {
    PLAYING( Exp* a1 ) : Exp1(a1) {}
    Val::TYPE type() const { return Val::INT; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp1::print( os << "playing" ); }
  };
  
  struct _or : public Exp2
  {
    _or( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "|" ); }
  };
  struct _and : public Exp2
  {
    _and( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "&" ); }
  };
  struct _eq : public Exp2
  {
    _eq( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "==" ); }
  };
  struct _ne : public Exp2
  {
    _ne( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "!=" ); }
  };
  struct _lt : public Exp2
  {
    _lt( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "<" ); }
  };
  struct _le : public Exp2
  {
    _le( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "<=" ); }
  };
  struct _gt : public Exp2
  {
    _gt( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << ">" ); }
  };
  struct _ge : public Exp2
  {
    _ge( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return Val::BOOL; }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << ">=" ); }
  };

  struct _neg : public Exp1
  {
    _neg( Exp* a1 ) : Exp1(a1) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp1::print( os << "!" ); }
  };
  struct _add : public Exp2
  {
    _add( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "+" ); }
  };
  struct _sub : public Exp2
  {
    _sub( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "-" ); }
  };
  struct _mul : public Exp2
  {
    _mul( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "*" ); }
  };
  struct _div : public Exp2
  {
    _div( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "/" ); }
  };
  struct _mod : public Exp2
  {
    _mod( Exp* a1, Exp* a2 ) : Exp2(a1,a2) {}
    Val::TYPE type() const { return a1->type(); }
    Val eval ( const Client* C, const Request& R ) const;
    std::ostream& print( std::ostream& os ) const { return Exp2::print( os << "%" ); }
  };
  
}

class VC_Formula
{
public:
   VC_Formula() : e(0) {}
   VC_Formula( const VC_Formula& F );
  ~VC_Formula();

  bool parse( std::ostream& os, const String& S );
  bool eval ( const Client* C, const Request& R ) const;

  void erase();
  
  std::ostream& print( std::ostream& os ) const;
  std::ostream& save ( std::ostream& os ) const;
  std::istream& load ( std::istream& is );
  
private:
  VC_Formula& operator= ( const VC_Formula& );

  String       s; // this is the text from which we have parsed e!
  VC_Exp::Exp* e;
};

inline std::ostream& operator<< ( std::ostream& os, const VC_Formula& v )
{
  return v.print( os );
}

#endif

//: VC_Formula.H (eof) (c) Igor
