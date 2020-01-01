#include <cstdio>
#include <string>
#include <unordered_map>
#include "../cmdhelper.h"
#include <vector>
#include <Loader.h>
#include <MC.h>
#include <unistd.h>
#include <cstdarg>

#include "../base/base.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gui.h"
using std::string;
using std::unordered_map;

extern "C"
{
    BDL_EXPORT void mod_init(std::list<string> &modlist);
}
extern void load_helper(std::list<string> &modlist);
struct FText
{
    float x, y, z;
    int dim;
    unsigned long eid;
    string text;
    void packto(DataStream &ds) const
    {
        ds << x << y << z << eid << text << dim;
    }
    void unpack(DataStream &ds)
    {
        ds >> x >> y >> z >> eid >> text >> dim;
    }
};
vector<FText> txts;
LDBImpl db("data_v2/floatText");
void sendFText(FText const &x)
{
    //remove 0x0e
    MyPkt pk(12, [&](void *, BinaryStream &bs) {
        bs.writeUnsignedInt64(x.eid);
        bs.writeUnsignedInt64(114514); //uuid
        bs.writeStr(x.text);           //username
        bs.writeUnsignedVarInt64(114514);
        bs.writeUnsignedVarInt64(114514);
        bs.writeStr("wtfwtf"); //platid
        bs.writeVec3({x.x,x.y,x.z});
        bs.writeVec3({0, 0, 0});
        bs.writeVec3({0, 0, 0});
        bs.writeUnsignedVarInt(0); //item
        bs.writeUnsignedInt(4);
        bs.writeUnsignedVarInt(4);
        bs.writeUnsignedVarInt(4);
        bs.writeStr(x.text);
        bs.writeUnsignedVarInt(38);
        bs.writeUnsignedVarInt(3);
        bs.writeFloat(0.001);
        bs.writeUnsignedVarInt(53);
        bs.writeUnsignedVarInt(3);
        bs.writeFloat(0.001);
        bs.writeUnsignedVarInt(54);
        bs.writeUnsignedVarInt(3);
        bs.writeFloat(0.001);
        for (int i = 0; i < 5; ++i)
            bs.writeUnsignedVarInt(0);
        bs.writeSignedInt64(114514);
        bs.writeUnsignedVarInt(0);
        bs.writeStr("114514");
        bs.writeSignedInt(114514);
    });
    auto usr=getSrvLevel()->getUsers();
    for(auto& i:*usr){
        if(i->getDimensionId()==x.dim){

        }
    }
}
void cm(argVec &a, CommandOrigin const &b, CommandOutput &c)
{
    
}
void mod_init(std::list<string> &modlist)
{
    printf("[FloatingText] Loaded " BDL_TAG "\n");
    load_helper(modlist);
    register_cmd("g", cm, "debug", 1);
}
