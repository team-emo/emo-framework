#ifndef EMO_DATABASE_H
#define EMO_DATABASE_H
namespace emo {
    class Database {
    public:
        Database();
        ~Database();
        std::string getDatabasePath(std::string name);
        std::string createDatabase(std::string name, jint mode);
    };
}
#endif