#ifndef DATACARDSRECORD_HH
#define DATACARDSRECORD_HH 1

#include <TObject.h>

#include "dataCards.hh"

class dataCardsRecord : public TObject {

 public:
  dataCardsRecord();
  virtual ~dataCardsRecord() {}

  double           fetchValueDouble(const string& key);
  int              fetchValueInt(const string& key);
  bool             fetchValueBool(const string& key);
  TVector3         fetchValue3Vector(const string& key);
  string           fetchValueString(const string& key);
  vector<double>   fetchValueVector(const string&);

  InputDataCardsDouble   getCardsDouble()  {return cd_r;}
  InputDataCards3Vector  getCards3Vector() {return c3v_r;}
  InputDataCardsString   getCardsString()  {return cs_r;}
  InputDataCardsVector   getCardsVector()  {return cv_r;}
  InputDataCardsInt      getCardsInt()     {return ci_r;}
  InputDataCardsBool     getCardsBool()    {return cb_r;}

  void printToStream(ostream& stream);

 private:
  InputDataCardsString  cs_r;
  InputDataCardsInt     ci_r;
  InputDataCardsBool     cb_r;
  InputDataCardsDouble  cd_r;
  InputDataCards3Vector c3v_r;
  InputDataCardsVector  cv_r;

  //ClassDef(dataCardsRecord, 1);
};

#endif
