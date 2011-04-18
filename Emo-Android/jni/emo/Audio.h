
namespace emo {
    class Audio {
    public:
        bool isRunning();
        void close();
    protected:
        bool running;
    };
}
