#include "dataCardsRecord.hh"

dataCardsRecord::dataCardsRecord() {
  dataCards *cards_ = dataCards::getInstance();
  cs_r  = cards_->getCardsString();
  cb_r  = cards_->getCardsBool();
  ci_r  = cards_->getCardsInt();
  cd_r  = cards_->getCardsDouble();
  c3v_r = cards_->getCards3Vector();
  cv_r  = cards_->getCardsVector();
}

double dataCardsRecord::fetchValueDouble(const string& key) {
  InputDataCardsDouble::iterator i=cd_r.find(key);
  if (i == cd_r.end()) {
    string mesage = "Error fetching unknown double datacard "+key+".";
    string location = "double dataCardsRecord::fetchValueDouble(const string& key)";
    exit(0);
  }
  return i->second;
}

int dataCardsRecord::fetchValueInt(const string& key) {
 InputDataCardsInt::iterator i=ci_r.find(key);
 if( i == ci_r.end()) {
    string mesage = "Error fetching unknown int datacard "+key+".";
    string location = "int dataCardsRecord::fetchValueInt(const string& key)";
    exit(0);
 }
   return i->second;
}

bool dataCardsRecord::fetchValueBool(const string& key) {
 InputDataCardsBool::iterator i=cb_r.find(key);
 if( i == cb_r.end()) {
    string mesage = "Error fetching unknown int datacard "+key+".";
    string location = "int dataCardsRecord::fetchValueInt(const string& key)";
    exit(0);
 }
   return i->second;
}

TVector3 dataCardsRecord::fetchValue3Vector(const string& key) {
  InputDataCards3Vector::iterator i=c3v_r.find(key);
  if (i == c3v_r.end()) {
    string mesage = "Error fetching unknown 3Vector datacard "+key+".";
    string location = "TVector3 dataCardsRecord::fetchValue3Vector(const string& key)";
    exit(0);
  }
  return i->second;
}

string dataCardsRecord::fetchValueString(const string& key) {
  InputDataCardsString::iterator i=cs_r.find(key);
  if (i == cs_r.end()) {
    string mesage = "Error fetching unknown string datacard "+key+".";
    string location = "string dataCardsRecord::fetchValueString(const string& key)";
    exit(0);
  }
  return i->second;
}

vector<double> dataCardsRecord::fetchValueVector(const string& key) {
  InputDataCardsVector::iterator i=cv_r.find(key);
  if (i == cv_r.end()) {
    string mesage = "Error fetching unknown vector datacard. "+key+".";
    string location = "vector<double> dataCardsRecord::fetchValueVector(const string& key)";
    exit(0);
  }
  return i->second;
}

void dataCardsRecord::printToStream(ostream& stream) {
  stream << " --- DataCardsReord Dump --- " << endl;

  stream << endl << " cards string : " << endl;
  InputDataCardsString::iterator it_cs = cs_r.begin();
  for (unsigned int i=0; i < cs_r.size(); i++) {
    stream << "  " << it_cs->first << " " << it_cs->second << endl;
    it_cs++;
  }

  stream << endl << " cards bool : " << endl;
  InputDataCardsBool::iterator it_cb = cb_r.begin();
  for (unsigned int i=0; i < cb_r.size(); i++) {
    stream << "  " << it_cb->first << " " << it_cb->second << endl;
    it_cb++;
  }

  stream << endl << " cards int : " << endl;
  InputDataCardsInt::iterator it_ci = ci_r.begin();
  for (unsigned int i=0; i < ci_r.size(); i++) {
    stream << "  " << it_ci->first << " " << it_ci->second << endl;
    it_ci++;
  }

  stream << endl << " cards double : " << endl;
  InputDataCardsDouble::iterator it_cd = cd_r.begin();
  for (unsigned int i=0; i < cd_r.size(); i++) {
    stream << "  " << it_cd->first << " " << it_cd->second << endl;
    it_cd++;
  }

  stream << endl << " cards TVector3 : " << endl;
  InputDataCards3Vector::iterator it_c3v = c3v_r.begin();
  for (unsigned int i=0; i < c3v_r.size(); i++) {
    stream << "  " << it_c3v->first << "  (" << it_c3v->second.x() << ", "
    << it_c3v->second.y() << ", " << it_c3v->second.z() << ")" << endl;

    it_c3v++;
  }

  stream << endl << " cards vector : " << endl;
  InputDataCardsVector::iterator it_cv = cv_r.begin();
  for (unsigned int i=0; i < cv_r.size(); i++) {
    stream << "  " << it_cv->first << " ( ";
    for (unsigned int j=0; j<it_cv->second.size(); j++)
      stream << it_cv->second[j] << " ";
    stream << ")" << endl;

    it_cv++;
  }
  stream << " ---------------------- " << endl;
}

