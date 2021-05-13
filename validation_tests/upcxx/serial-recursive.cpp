#include <upcxx/upcxx.hpp>
#include <iostream>
#include <string>
#include <vector>

//SNIPPET
class custom_class_1 {
    public:
        std::string msg;
        custom_class_1() { }
        custom_class_1(const std::string &_msg) : msg(_msg) { }

        UPCXX_SERIALIZED_FIELDS(msg)
};

class custom_class_2 {
    public:
        std::vector<custom_class_1> msgs;
        custom_class_2() { }
        void add_msg(const std::string &m) { msgs.push_back(custom_class_1(m)); }

        UPCXX_SERIALIZED_FIELDS(msgs)
};
//SNIPPET

int main(void) {
    upcxx::init();

    int rank = upcxx::rank_me();
    int nranks = upcxx::rank_n();

    custom_class_1 msg(std::string("Howdy from rank ") + std::to_string(rank));
    upcxx::rpc(0, [] (const custom_class_1& msg) {
                std::cout << msg.msg << std::endl;
            }, msg).wait();
    upcxx::barrier();

    if (rank == 0) std::cout << std::endl;

    custom_class_2 custom_msgs;
    custom_msgs.add_msg("Howdy");
    custom_msgs.add_msg("yet");
    custom_msgs.add_msg("again");
    custom_msgs.add_msg("from");
    custom_msgs.add_msg("rank");
    custom_msgs.add_msg(std::to_string(rank));
    upcxx::rpc(0, [] (const custom_class_2& custom_msgs) {
                std::stringstream ss;
                for (auto i = custom_msgs.msgs.begin(),
                        e = custom_msgs.msgs.end(); i != e; i++) {
                    ss << " " << (*i).msg;
                }
                std::cout << ss.str() << std::endl;
            }, custom_msgs).wait();
    upcxx::barrier();

    if (rank == 0) {
        std::cout << "SUCCESS" << std::endl;
    }

    upcxx::finalize();

    return 0;
}
