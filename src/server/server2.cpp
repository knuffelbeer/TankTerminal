#include <functional>
#include <vector>
#define BUFFERALLOCATE 100
class Reader {
public:
  Reader();
  Reader(Reader &&) = default;
  Reader(const Reader &) = default;
  Reader &operator=(Reader &&) = default;
  Reader &operator=(const Reader &) = default;
  ~Reader();
  void process(std::function<void()> func);

private:
  std::vector<char> buffer;
  int idx{};
};

Reader::Reader() { buffer = std::vector<char>(BUFFERALLOCATE); }


  void Reader::process(std::function<void()> func){
					int i = 0;
					func = [&i](){};


	}

Reader::~Reader() {}
