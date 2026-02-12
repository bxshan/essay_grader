#include <bits/stdc++.h>
// ignore this v
using namespace std;
using ll = long long;
#define int long long
#define pb push_back
#define mp make_pair
#define endl '\n'
#define f first
#define s second 
#define INF 0x3f3f3f3f
#define all(x) (x).begin(), (x).end()
#define fileio(file) freopen(file ".in", "r", stdin); freopen(file ".out", "w", stdout)
#define pout(v); for (auto i : v) {cout << i << " ";} cout << endl;
#define FOR(i, n, m) for (int i = (n); i < (m); i++)
#define ROF(i, n, m) for (int i = (n); i > (m); i--)
// this doesnt exist ^

const unordered_set<string> NNN = {
  "very", "really", "get", "gets", "got", "gotten", "getting"};

const unordered_set<string> PREP = {
  "about", "above", "across", "after", "against", "along", "among", "around",
  "at", "before", "behind", "below", "beneath", "beside", "between", "beyond",
  "by", "down", "during", "except", "for", "from", "in", "inside", "into",
  "near", "of", "off", "on", "onto", "out", "outside", "over", "past",
  "through", "to", "toward", "towards", "under", "underneath", "until", "up",
  "upon", "with", "within", "without"};

unordered_set<string> dict;

string S;

// stole this from https://stackoverflow.com/questions/236129/how-do-i-iterate-over-the-words-of-a-string
template <typename Out>
void split(const string &s, char delim, Out result) {
  istringstream iss(s);
  string item;
  while (getline(iss, item, delim)) {
    *result++ = item;
  }
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, back_inserter(elems));
  return elems;
}

vector<string> jsplit(const string &s) {
  vector<string> J;
  string curr;
  FOR(i, 0, s.size()) {
    if (s[i] == '\n') {
      // apparently there is a builtin method for everything
      int l = curr.find_first_not_of(" \t\n\r");
      if (l != string::npos) {
        int r = curr.find_last_not_of(" \t\n\r");
        string trimmed = curr.substr(l, r - l + 1);
        if (!trimmed.empty()) J.pb(trimmed);
      }
      curr.clear();
      continue;
    }
    curr += s[i];
    if (s[i] == '.' || s[i] == '!' || s[i] == '?') {
      while (i + 1 < s.size() &&
          (s[i + 1] == '.' || s[i + 1] == '!' || s[i + 1] == '?')) {
        curr += s[++i];
      }

      int l = curr.find_first_not_of(" \t\n\r");
      if (l != string::npos) {
        int r = curr.find_last_not_of(" \t\n\r");
        string trimmed = curr.substr(l, r - l + 1);
        if (!trimmed.empty()) J.pb(trimmed);
      }
      curr.clear();
    }
  }
  return J;
}

// strip whitespace
string stripw(const string& w) {
  string r;
  for(char c : w) if(isalpha((unsigned char)c)) r += c;
  return r;
}

// escape characters for json
string jsonesc(const string& s) {
  string r;
  for(char c : s) {
    if(c == '"') r += "\\\"";
    else if(c == '\\') r += "\\\\";
    else if(c == '\n') r += "\\n";
    else r += c;
  }
  return r;
}

// trigram jaccard similarity for plagiarism detection
// take consecutive sets of 3 words from each, take the # matching unique / total
double similarity(const string& t1, const string& t2) {
  auto getTrigrams = [](const string& t) {
    // extract lowercase alpha words
    vector<string> words;
    string cur;
    for(char c : t) {
      if(isalpha((unsigned char)c)) cur += tolower((unsigned char)c);
      else if(!cur.empty()) { words.pb(cur); cur.clear(); }
    }
    if(!cur.empty()) words.pb(cur);
    unordered_set<string> tri;
    FOR(i, 0, (int)words.size() - 2)
      tri.insert(words[i] + " " + words[i+1] + " " + words[i+2]);
    return tri;
  };
  auto a = getTrigrams(t1), b = getTrigrams(t2);
  if(a.empty() && b.empty()) return 1.0;
  if(a.empty() || b.empty()) return 0.0;
  int isect = 0;
  for(auto& t : a) if(b.count(t)) isect++;
  return (double)isect / (a.size() + b.size() - isect);
}

// c++ dictionary doesnt include suffixes of words: e.g. writing is unrecognized
// so cut off all the suffixes and count from there
// spaghettiest spaghetti code every
bool spellchk(const string& w) {
  if (w.empty() || dict.count(w)) return 1;
  int n = w.size();
  // suffixes
  // -s
  if (n > 2 && w.back() == 's' && dict.count(w.substr(0, n-1))) return 1;
  // -es
  if (n > 3 && w.substr(n-2) == "es" && dict.count(w.substr(0, n-2))) return 1;
  // -ies->y
  if (n > 4 && w.substr(n-3) == "ies" && dict.count(w.substr(0, n-3) + "y")) return 1;
  // -ed
  if (n > 3 && w.substr(n-2) == "ed" && dict.count(w.substr(0, n-2))) return 1;
  // -ed->e
  if (n > 3 && w.substr(n-2) == "ed" && dict.count(w.substr(0, n-1))) return 1;
  // -ied->y
  if (n > 4 && w.substr(n-3) == "ied" && dict.count(w.substr(0, n-3) + "y")) return 1;
  // -ing
  if (n > 4 && w.substr(n-3) == "ing" && dict.count(w.substr(0, n-3))) return 1;
  // -ing->e
  if (n > 4 && w.substr(n-3) == "ing" && dict.count(w.substr(0, n-3) + "e")) return 1;
  // -ly
  if (n > 3 && w.substr(n-2) == "ly" && dict.count(w.substr(0, n-2))) return 1;
  // -er: bigger->big (double consonant)
  if (n > 3 && w.substr(n-2) == "er" && dict.count(w.substr(0, n-2))) return 1;
  // -er->e
  if (n > 3 && w.substr(n-2) == "er" && dict.count(w.substr(0, n-1))) return 1;
  // -est
  if (n > 4 && w.substr(n-3) == "est" && dict.count(w.substr(0, n-3))) return 1;
  // -est->e
  if (n > 4 && w.substr(n-3) == "est" && dict.count(w.substr(0, n-2))) return 1;
  
  return 0;
}

signed main() {
  ios::sync_with_stdio(0);
  cin.tie(nullptr);

  // load dictionary
  {
    ifstream f("/usr/share/dict/words");
    string line;
    while(getline(f, line)) {
      while(!line.empty() && isspace((unsigned char)line.back())) line.pop_back();
      transform(all(line), line.begin(), [](unsigned char c){ return tolower(c); });
      if(!line.empty()) dict.insert(line);
    }
  }

  // read all previous into stdin, format: essay\0name1\0essay1\0name2\0essay2\0...
  string input;
  {
    ostringstream ss;
    ss << cin.rdbuf();
    input = ss.str();
  }
  // split essays/name on null bytes
  vector<string> parts;
  {
    string cur;
    for(char c : input) {
      if(c == '\0') { parts.pb(cur); cur.clear(); }
      else cur += c;
    }
    parts.pb(cur);
  }
  // parts[2n] is the essay of the nth student, parts[2n+1] is nth students name
  // 0th student is current, take their essay to grade
  S = parts[0];

  // plagiarism check against previous essays
  vector<string> r;
  for(int i = 1; i + 1 < (int) parts.size(); i += 2) {
    string prevName = parts[i];
    string prevEssay = parts[i+1];
    int simsc = similarity(S, prevEssay);
    if(simsc > 0.8) {
      cout << "{\"grade\":0,\"reasons\":[\""
           << jsonesc("uh oh!! plagiarism detected: too similar to ") + prevName
           << jsonesc(", with score ") + to_string(simsc)
           << "\"]}" << '\n';
      return 0;
    }
  }

  // make W, a vector of sentences that is a vector of words
  transform(all(S), S.begin(), [](unsigned char c){ return tolower(c); });

  vector<vector<string>> W;
  for(auto& j : jsplit(S)) {
    vector<string> ws;
    for(auto& tok : split(j, ' ')) {
      string c = stripw(tok);
      if(!c.empty()) ws.pb(c);
    }
    if(!ws.empty()) W.pb(ws);
  }

  // init score
  int s = 100;

  // -1 per nnn
  for(auto j : W) {
    for(string w : j) {
      auto it = NNN.find(w);
      if (it != NNN.end()) {
        s--;
        r.pb("uh oh!! nnn found: " + *it);
      }
    }
  }
  
  // -1 per misspelled word
  int sperr = 0;
  for(auto& j : W) {
    for(auto& raW : j) {
      // strip nonalpha
      string w;
      for(char c : raW) if(isalpha((char) c)) w += c;
      if(w.empty()) continue;
      if(!spellchk(w)) {
        sperr++;
        r.pb("uh oh!! misspelled: " + w);
      }
    }
  }
  s -= sperr;

  // 3% off for a pair of sentences starting with the same word, as long as they are separated by no more than 3 sentences (exclusive). Do not double-count the same pair.

  vector<string> d;
  d.reserve(W.size()); 
  transform(all(W), 
      back_inserter(d), 
      [](const vector<string>& iv) {
      return iv.front(); 
      });

  int deduct = 0;
  auto it = d.begin();
  while(it != d.end()) {
    auto bound = min(it + 4, d.end());
    auto jt = find(it + 1, bound, *it);
    while (jt < bound) {
      deduct++;
      r.pb("uh oh!! " +
          to_string(jt-it) +
          " sentence first word matches " +
          to_string(it-d.begin()) +
          " sentence");

      jt = find(jt + 1, bound, *it);
    }
    it++;
  }
  s -= 3 * deduct;

  // -5 per J end w/ prep
  FOR(i, 0, W.size()) {
    auto j = W[i];
    string w = j.back();
    if (PREP.count(w)) {
      r.pb("uh oh!! " + to_string(i) + " ends with preposition \"" + w + " \"");
      s -= 5;
    }
  }

  // wcnt
  int wcnt = 0;
  for(auto j : W) wcnt += j.size();
  if (wcnt < 500 || wcnt > 1000) {
    s -= 50;
    r.pb("word count not in range: " + to_string(wcnt));
  }

  // min at -200
  if (s < -200) {
    r.pb("score " + to_string(s) + " too low, set at -200");
    s = -200;
  }

  // feedback json
  cout << "{\"grade\":" << s << ",\"reasons\":[";
  FOR(i, 0, r.size()) {
    if(i) cout << ",";
    cout << "\"" << jsonesc(r[i]) << "\"";
  }
  cout << "]}" << '\n';

  return 0;
}
