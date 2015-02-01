#include "dataCards.hh"
#include "dataCardsRecord.hh"

unsigned int dataCards::count_ = 0;
dataCards* dataCards::instance_ = NULL;
InputDataCardsDouble  dataCards::cd_;
InputDataCards3Vector dataCards::c3v_;
InputDataCardsString  dataCards::cs_;
InputDataCardsVector  dataCards::cv_;
InputDataCardsInt     dataCards::ci_;
InputDataCardsBool    dataCards::cb_;

dataCards* dataCards::getInstance() {
  if (instance_) {
    instance_ = new dataCards();
  }

  return instance_;
}

dataCards::dataCards() {
  count_++;
  if (count_ > 1) {
    string mesage = "Trying to create a multiple instances of a static singleton class.";
    string location = "dataCards::dataCards()";
  }
}

dataCards::~dataCards() {
  count_--;
}

void dataCards::AddDataCardString(string aName, string aDefaultValue) {
  cs_[ aName ] = aDefaultValue;
}

void dataCards::AddDataCardInt(string aName, int aDefaultValue) {
  ci_[ aName ] = aDefaultValue;
}

void dataCards::AddDataCardBool(string aName, bool aDefaultValue) {
  cb_[ aName ] = aDefaultValue;
}

void dataCards::AddDataCardDouble(string aName, double aDefaultValue) {
  cd_[ aName ] = aDefaultValue;
}

void dataCards::AddDataCardVector(string aName, vector< double > aDefaultValue) {
  cv_[ aName ] = aDefaultValue;
}

void dataCards::AddDataCardVector(string aName, const unsigned int aSize, double* aDefaultValue) {
  vector<double> dVector;
  dVector.assign( aDefaultValue, aDefaultValue + aSize );
  cv_[ aName ] = dVector;
}

double dataCards::fetchValueDouble(const string& key) {
  InputDataCardsDouble::iterator i=cd_.find(key);
  if (i == cd_.end()) {
    string mesage = "Error fetching unknown double datacard "+key+".";
    string location = "double dataCards::fetchValueDouble(const string& key)";
    exit(0);
  }
  return i->second;
}

int dataCards::fetchValueInt(const string& key) {
 InputDataCardsInt::iterator i=ci_.find(key);
 if( i == ci_.end()) {
    string mesage = "Error fetching unknown int datacard "+key+".";
    string location = "int dataCards::fetchValueInt(const string& key)";
    exit(0);
 }
   return i->second;
}

bool dataCards::fetchValueBool(const string& key) {
 InputDataCardsBool::iterator i=cb_.find(key);
 if( i == cb_.end()) {
    string mesage = "Error fetching unknown bool datacard "+key+".";
    string location = "int dataCards::fetchValueBool(const string& key)";
    exit(0);
 }
   return i->second;
}

TVector3 dataCards::fetchValue3Vector(const string& key) {
  InputDataCards3Vector::iterator i=c3v_.find(key);
  if (i == c3v_.end()) {
    string mesage = "Error fetching unknown 3Vector datacard "+key+".";
    string location = "TVector3 dataCards::fetchValue3Vector(const string& key)";
    exit(0);
  }
  return i->second;
}

string dataCards::fetchValueString(const string& key) {
  InputDataCardsString::iterator i=cs_.find(key);
  if (i == cs_.end()) {
    string mesage = "Error fetching unknown string datacard "+key+".";
    string location = "string dataCards::fetchValueString(const string& key)";
    exit(0);
  }
  return i->second;
}

vector<double> dataCards::fetchValueVector(const string& key) {
  InputDataCardsVector::iterator i=cv_.find(key);
  if (i == cv_.end()) {
    string mesage = "Error fetching unknown vector datacard. "+key+".";
    string location = "vector<double> dataCards::fetchValueVector(const string& key)";
    exit(0);
  }
  return i->second;
}

void dataCards::readKeys(string fileName) {

  std::cout << "Reading cards from file " << fileName << "." << std::endl;
  std::ifstream in(fileName.c_str());
  if (in)
    readKeys(in);
  else {
    string mesage = "dataCards File " + fileName + " not found.";
    string location = "void dataCards::readKeys(string fileName)";
    exit(0);
  }
}

void dataCards::readKeys(std::istream& in) {
  string s;
  string t;
  string c;

  while (in) {
    c="";
    t="";
    getline(in,c);
    std::istringstream is(c);
    is >> t;
    if (t.length() == 0) continue;
    if (t[0] == '#') continue; // Comments cards ignored
    if (t[0] == '!') continue; // MCFast style comment
    int ok = 0;

    InputDataCardsDouble::iterator i = cd_.find(t);
    if ( i != cd_.end()) {
      double d;
      is >> d;
      cd_[t] = d;
      ok = 1;
    }

    if( ok == 0)
    {
      InputDataCardsBool::iterator i1 = cb_.find(t);
      if(i1 != cb_.end()){
        bool b;
        is >> b;
        cb_[t] = b;
        ok =1;
      }
    }

    if( ok == 0)
    {
      InputDataCardsInt::iterator i1 = ci_.find(t);
      if(i1 != ci_.end()){
        int ii;
        is >> ii;
        ci_[t] = ii;
        ok =1;
      }
    }

    if (ok == 0) {
      std::istringstream is2(c);
      is2 >> t;
      InputDataCardsString::iterator i2 = cs_.find(t);
      if ( i2 != cs_.end()) {
        string s2;
        is>> s2;
        cs_[t] = s2;
        ok = 1;
      }
    }

    if (ok == 0) {
      std::istringstream iv(c);
      iv >> t;
      InputDataCards3Vector::iterator i2 = c3v_.find(t);
      if ( i2 != c3v_.end()) {
        double x, y, z;
        iv >> x >> y >> z;
        TVector3 v(x, y, z);
        c3v_[t] = v;
        ok = 1;
      }
    }

    if (ok == 0) {
      InputDataCardsVector::iterator i2 = cv_.find(t);
      if ( i2 != cv_.end()) {
        int nentries;
        is >> nentries;
        vector<double> cvec(nentries);
        for(int i=0; i<nentries; i++) {
          double vd;
          if ( is.good() ) {
            is >> vd;
          } else {
            string mesage = "Error reading key " + t + ". Bad data format.";
            string location = "void dataCards::readKeys(std::istream& in)";
            exit(0);
          }
          cvec[i] = vd;
        }
        cv_[t] = cvec;
        ok = 1;
      }
    }
    if (ok != 1){
      string mesage = "Failed to recognise datacard " + t + ".";
      string location = "void dataCards::readKeys(std::istream& in)";
      exit(0);
    }
  }
}

void dataCards::printToStream(ostream& stream) {
  stream << "-------- dataCards --------" << endl;

  stream << endl << " cards string : " << endl;
  InputDataCardsString::iterator it_cs = cs_.begin();
  for (unsigned int i=0; i < cs_.size(); i++) {
    stream << "  " << it_cs->first << " " << it_cs->second << endl;
    it_cs++;
  }

  stream << endl << " cards bool : " << endl;
  InputDataCardsBool::iterator it_cb = cb_.begin();
  for (unsigned int i=0; i < cb_.size(); i++) {
    stream << "  " << it_cb->first << " " << it_cb->second << endl;
    it_cb++;
  }

  stream << endl << " cards int : " << endl;
  InputDataCardsInt::iterator it_ci = ci_.begin();
  for (unsigned int i=0; i < ci_.size(); i++) {
    stream << "  " << it_ci->first << " " << it_ci->second << endl;
    it_ci++;
  }

  stream << endl << " cards double : " << endl;
  InputDataCardsDouble::iterator it_cd = cd_.begin();
  for (unsigned int i=0; i < cd_.size(); i++) {
    stream << "  " << it_cd->first << " " << it_cd->second << endl;
    it_cd++;
  }

  stream << endl << " cards TVector3 : " << endl;
  InputDataCards3Vector::iterator it_c3v = c3v_.begin();
  for (unsigned int i=0; i < c3v_.size(); i++) {
    stream << "  " << it_c3v->first << "  (" << it_c3v->second.x() << ", "
    << it_c3v->second.y() << ", " << it_c3v->second.z() << ")" << endl;

    it_c3v++;
  }

  stream << endl << " cards vector : " << endl;
  InputDataCardsVector::iterator it_cv = cv_.begin();
  for (unsigned int i=0; i < cv_.size(); i++) {
    stream << "  " << it_cv->first << " ( ";
    for (unsigned int j=0; j<it_cv->second.size(); j++)
      stream << it_cv->second[j] << " ";
    stream << ")" << endl;

    it_cv++;
  }
  stream << "---------------------------" << endl;
}



