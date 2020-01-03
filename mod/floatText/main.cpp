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
const unsigned long bbid=1145141919ull;
string BossBar="ksksksk";
void MoveBB(ServerPlayer* sp){
    MyPkt pk(0x12,[&](void*,BinaryStream& bs){
        bs.writeUnsignedVarInt64(bbid);
        bs.writeByte(3);
        auto& pos=sp->getPos();
        bs.writeVec3({pos.x,-10,pos.z});
        bs.writeUnsignedInt(0); //3*bytes
    });
    sp->sendNetworkPacket(pk);
}
void SendBB(ServerPlayer* sp,string_view bbar,bool show=true){
    MyPkt pk(0x4a,[&](void*,BinaryStream& bs){
        bs.writeUnsignedVarInt64(bbid);
        bs.writeUnsignedVarInt(show?0:2);
        bs.writeStr(bbar);
        bs.writeFloat(0.5);
    });
    sp->sendNetworkPacket(pk);
}
void CreateEntBB(ServerPlayer* sp){
    MyPkt pk(0x0d,[&](void*,BinaryStream& bs){
        bs.writeUnsignedVarInt64(bbid);
        bs.writeUnsignedVarInt64(bbid);
        bs.writeStr("minecraft:pig");
        auto& pos=sp->getPos();
        bs.writeVec3({pos.x,-10,pos.z});
         bs.writeVec3({0,0,0});
         bs.writeVec3({0,0,0});
         bs.writeUnsignedVarInt(0); //attr
         bs.writeUnsignedVarInt(0); //meta
         bs.writeUnsignedVarInt(0); //link
    });
    sp->sendNetworkPacket(pk);
}
void ReShowBB(ServerPlayer* sp){
    MoveBB(sp);
    SendBB(sp,BossBar);
}
void cm(argVec &a, CommandOrigin const &b, CommandOutput &c)
{
    if(a[0]=="all"){
        auto* x=getSrvLevel()->getUsers();
        for(auto& i:*x){
            auto sp=i.get();
            SendBB(sp,"",false);
            CreateEntBB(sp);
            SendBB(sp,a[1],true);
        }
        c.success();
        return;
    }
    auto sp=getuser_byname(a[0]);//getSP(b.getEntity());
    if(sp){
        SendBB(sp,"",false);
        CreateEntBB(sp);
        SendBB(sp,a[1],true);
        c.success();
    }
}
void cm2(argVec &a, CommandOrigin const &b, CommandOutput &c)
{
    if(a[0]=="all"){
        auto* x=getSrvLevel()->getUsers();
        for(auto& i:*x){
            auto sp=i.get();
            SendBB(sp,"",false);
        }
        c.success();
        return;
    }
    auto sp=getuser_byname(a[0]);//getSP(b.getEntity());
    if(sp){
        SendBB(sp,"",false);
        c.success();
    }
}
void mod_init(std::list<string> &modlist)
{
    printf("[FloatingText] Loaded " BDL_TAG "\n");
    load_helper(modlist);
    register_cmd("sendbb", cm, "send bb", 1);
    register_cmd("hidebb", cm2, "hide bb", 1);
}
