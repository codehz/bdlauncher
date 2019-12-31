#include<cstdio>
#include<list>
#include<forward_list>
#include<string>
#include<unordered_map>
#include"../cmdhelper.h"
#include<vector>
#include<Loader.h>
#include<MC.h>
#include<unistd.h>
#include<cstdarg>

#include"../base/base.h"
#include<cmath>
#include<deque>
#include<dlfcn.h>
#include<string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "gui.h"
using std::string;
using std::unordered_map;
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define dbg_printf(...) {}
//#define dbg_printf printf
extern "C" {
    BDL_EXPORT void mod_init(std::list<string>& modlist);
}
extern void load_helper(std::list<string>& modlist);

using namespace rapidjson;
using std::vector;
using std::unordered_map;
unordered_map<int,SharedForm*> id_forms;
AllocPool<SharedForm> FormMem;
SharedForm* getForm(string_view title,string_view cont,bool isInp){
//	return new SharedForm(title,cont,true,isInp);
    return FormMem.get(title,cont,true,isInp); //need free
}
static void relForm(SharedForm* sf){
    if(sf->needfree){
        FormMem.release(sf);
    }
}
struct GUIPK{
    MyPkt* pk;
    string_view fm;
    int id;
    GUIPK(){
        pk=new MyPkt(100,[this](void* x,BinaryStream& b)->void{
            b.writeUnsignedVarInt(id);
            b.writeUnsignedVarInt(fm.size());
            b.write(fm.data(),fm.size());
        });
    }
    void send(ServerPlayer& sp,string_view st,int i){
        fm=st,id=i;
        sp.sendNetworkPacket(*pk);
    }
} gGUIPK;
static inline void sendStr(ServerPlayer& sp,string_view fm,int id){
    gGUIPK.send(sp,fm,id);
}
static int autoid;
BDL_EXPORT void sendForm(ServerPlayer& sp,SharedForm* fm){
    if(id_forms.size()>128){
        for(auto& i:id_forms){
            delete i.second;
        }
        id_forms.clear();
        printf("[GUI] Warning!Form Spam Detected!Clearing Form datas.Last Player %s\n",sp.getName().c_str());
    }
    auto x=fm->serial();
    fm->fid=++autoid;
    id_forms[autoid]=fm;
    sendStr(sp,x,autoid);
}
THook(void*,_ZN20ServerNetworkHandler6handleERK17NetworkIdentifierRK23ModalFormResponsePacket,ServerNetworkHandler* sh,NetworkIdentifier const& iden,Packet* pk){
    ServerPlayer* p=sh->_getServerPlayer(iden,pk->getClientSubId());
    if(p){
        int id=access(pk,int,36);
        auto it=id_forms.find(id);
        if(it!=id_forms.end()){
            it->second->process(p,access(pk,string,40));
            relForm(it->second);
            id_forms.erase(it);
        }
    }
    return nullptr;
}

void gui_ChoosePlayer(ServerPlayer* sp,string_view text,string_view title,std::function<void(ServerPlayer*,string_view)> const& cb){
    vector<string> names;
    get_player_names(names);
    auto fm=getForm(title,text,false);
    fm->cb=[cb](ServerPlayer* sp,string_view sv,int x){
        cb(sp,sv);
    };
    for(auto& i:names){
        fm->addButton(i);
    }
    sendForm(*sp,fm);
}
void cm(argVec& a,CommandOrigin const& b,CommandOutput& c){
    auto sp=getSP(b.getEntity());
    ItemStack is;
    auto pis=createItemStack("dirt",10,&is);
    giveItem(*sp,pis);
    printf("res %d\n",takeItem(*sp,&sp->getCarriedItem()));
    auto eid=summon(sp->getRegion(),sp->getPos(),"zombie");
    Actor* act=getSrvLevel()->fetchEntity(eid,false);
    if(act){
        printf("get act\n");
        act->setNameTag("kksk");
        //act->setSize(10,10);
        act->setAutonomous(true);
        act->setNameTagVisible(true);
        act->setScoreTag("kksk");
        is.~ItemStack();
        pis=createItemStack("diamond_sword",1,&is);
        act->setCarriedItem(*pis);
    }
    return;
    auto& pos=sp->getPos();
    MyPkt pk(12,[&](void*,BinaryStream&bs){
    bs.writeUnsignedInt64(114514);
    bs.writeUnsignedInt64(114514); //uuid
    bs.writeStr("nmsl"); //username
        bs.writeUnsignedVarInt64(114514);
        bs.writeUnsignedVarInt64(114514);
        bs.writeStr("minecraft:player"); //platid
        bs.writeVec3(pos);
        bs.writeVec3({0,0,0});
        bs.writeVec3({0,0,0});
        bs.writeUnsignedVarInt(0); //item
       /* {
            bs.writeStr("wtf");
            bs.writeVec3({0,0,0});
        }*/
        bs.writeUnsignedInt(4);
        /*   public static final int DATA_TYPE_BYTE = 0;
    public static final int DATA_TYPE_SHORT = 1;
    public static final int DATA_TYPE_INT = 2;
    public static final int DATA_TYPE_FLOAT = 3;
    public static final int DATA_TYPE_STRING = 4;
    public static final int DATA_TYPE_NBT = 5;
    public static final int DATA_TYPE_POS = 6;
    public static final int DATA_TYPE_LONG = 7;
    public static final int DATA_TYPE_VECTOR3F = 8;
*/

        //meta
        /*    
        public static final int DATA_SCALE = 38; //float
            public static final int DATA_BOUNDING_BOX_WIDTH = 53; //float
    public static final int DATA_BOUNDING_BOX_HEIGHT = 54; //float
public static final int DATA_NAMETAG = 4; //string

            metadata.putLong(Entity.DATA_FLAGS, flags)
                .putLong(Entity.DATA_LEAD_HOLDER_EID,-1)
                .putFloat(Entity.DATA_SCALE, 0.01f) //zero causes problems on debug builds?
                .putFloat(Entity.DATA_BOUNDING_BOX_HEIGHT, 0.01f)
                .putFloat(Entity.DATA_BOUNDING_BOX_WIDTH, 0.01f);
        if (!Strings.isNullOrEmpty(title)) {
            metadata.putString(Entity.DATA_NAMETAG, title);
        }
*/
bs.writeUnsignedVarInt(4);bs.writeUnsignedVarInt(4);bs.writeStr("nmsl");
bs.writeUnsignedVarInt(38);bs.writeUnsignedVarInt(3);bs.writeFloat(5);
bs.writeUnsignedVarInt(53);bs.writeUnsignedVarInt(3);bs.writeFloat(5);
bs.writeUnsignedVarInt(54);bs.writeUnsignedVarInt(3);bs.writeFloat(5);
        for(int i=0;i<5;++i) bs.writeUnsignedVarInt(0);
        bs.writeSignedInt64(114514);
        bs.writeUnsignedVarInt(0);
        bs.writeStr("114514");
        bs.writeSignedInt(114514);
    });
    sp->sendNetworkPacket(pk);
}
void mod_init(std::list<string>& modlist) {
    printf("[GUI] Loaded " BDL_TAG "\n");
    load_helper(modlist);
    register_cmd("Z",cm,"debug",1);
}
