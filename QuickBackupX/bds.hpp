// Created by JasonZYT on 2021/02/05
#pragma once
//#ifndef QBX_BDSHPP_INCLUDED
//#define QBX_BDSHPP_INCLUDED
#include "pch.h"
#include <string>
#include <map>
#include <time.h>

namespace QuickBackupX
{
#pragma region Player
	struct Vec3 {
		float x;
		float y;
		float z;

		std::string toJsonString() {
			char str[256];
			sprintf_s(str, "%d.%d.%d", (int)x, (int)y, (int)z);
			return std::string(str);
		}
		std::string toString() {
			char str[256];
			sprintf_s(str, "%d,%d,%d", (int)x, (int)y, (int)z);
			return std::string(str);
		}
	};

	struct MCUUID {
		// 取uuid字符串
		std::string toString() {
			std::string s;
			SYMCALL<std::string&>("?asString@UUID@mce@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this, &s);
			return s;
		}
	};
	struct Actor {
		// 获取生物名称信息
		std::string getNameTag() {
			return SYMCALL<std::string&>("?getNameTag@Actor@@UEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this);
		}
		// 获取生物当前所处维度ID
		int getDimensionId() {
			int dimensionId;
			SYMCALL<int&>("?getDimensionId@Actor@@UEBA?AV?$AutomaticID@VDimension@@H@@XZ", this, &dimensionId);
			return dimensionId;
		}
		// 获取生物当前所在坐标
		Vec3* getPos() {
			return SYMCALL<Vec3*>("?getPos@Actor@@UEBAAEBVVec3@@XZ", this);
		}
		// 获取生物类型
		std::string getTypeName() {
			std::string actor_typename;
			SYMCALL<std::string&>("?getEntityName@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVActor@@@Z", &actor_typename, this);
			return actor_typename;
		}
		// 获取生物类型ID
		int getTypeID() {
			return SYMCALL<__int64>("?getEntityTypeId@Actor@@UEBA?AW4ActorType@@XZ", this);
			//return *(unsigned int *)(this + 908);
		}
		// 是否悬空
		const BYTE isStand() {				// IDA MovePlayerPacket::MovePlayerPacket
			return *reinterpret_cast<BYTE*>(reinterpret_cast<VA>(this) + 448);
		}
	};

	struct Player : Actor {
		// 取uuid
		MCUUID* getUuid() {
			return (MCUUID*)((VA)this + 2720);
		}
		// 取xuid
		std::string getXuid(VA level) {
			return SYMCALL<std::string&>("?getPlayerXUID@Level@@QEBAAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AEBVUUID@mce@@@Z", level, (VA)this + 2720);
		}
		// 获取网络标识符
		VA getNetId() {
			return (VA)this + 2944;		// IDA ServerPlayer::setPermissions
		}
		// 发送数据包
		VA sendPacket(VA pkt) {
			return SYMCALL<VA>("?sendNetworkPacket@ServerPlayer@@UEBAXAEAVPacket@@@Z", this, pkt);
		}
	};
#pragma endregion

#pragma region Block
	struct BPos3 {
		INT32 x;
		INT32 y;
		INT32 z;
	public:
		std::string toJsonString() {
			char str[256];
			sprintf_s(str, "%d.%d.%d", x, y, z);
			return std::string(str);
		}
		std::string toString() {
			char str[256];
			sprintf_s(str, "%d,%d,%d", x, y, z);
			return std::string(str);
		}
	};
	struct BlockPos : BPos3 {
		// 获取坐标数组头
		BPos3* getPosition() const {
			return (BPos3*)this;
		}
		/*通过 BDS 的指令原生输出
		std::string toString() {
			std::string s;
			SYMCALL<string&>("?toString@BlockPos@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ", this, &s);
			return s;
		}*/
		std::string toString()
		{
			return std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z);
		}
		// 通过 Vec3 构造 BlockPos
		BlockPos(const void* vec3) {
			auto v = (float*)vec3;
			x = v ? (int)v[0] : 0;
			y = v ? (int)v[1] : 0;
			z = v ? (int)v[2] : 0;
		}
		// 通过 double 构造 BlockPos
		BlockPos(double x2, double y2, double z2) {
			x = (int)x2;
			y = (int)y2;
			z = (int)z2;
		}
		BlockPos() {
			memset(this, 0, sizeof(BlockPos));
		}
	};
	struct BlockLegacy {
		// 获取方块名
		auto getFullName() const {				// IDA BlockLegacy::~BlockLegacy
			return (std::string&)*(__int64*)((__int64)this + 104);
		}
		// 获取方块ID号
		auto getBlockItemID() const {			// IDA BlockLegacy::BlockLegacy VanillaItems::serverInitCreativeItemsCallback Item::beginCreativeGroup "itemGroup.name.planks"
			short v3 = *(short*)((VA)this + 312);
			if (v3 < 0x100) {
				return v3;
			}
			return (short)(255 - v3);
		}
	};

	struct Block {
		// 获取源
		const BlockLegacy* getLegacyBlock() const {			// IDA LegacyStructureTemplate::_mapToProperty "waterlogged"
			return SYMCALL<BlockLegacy*>("?getLegacyBlock@Block@@QEBAAEBVBlockLegacy@@XZ", this);
		}
	};

	struct Dimension {
		// 获取方块源
		VA getBlockSource() {					// IDA Level::tickEntities
			return *((VA*)this + 10);
		}
	};

	struct BlockActor {
		// 取方块
		Block* getBlock() {
			return *reinterpret_cast<Block**>(reinterpret_cast<VA>(this) + 16);
		}
		// 取方块位置
		BlockPos* getPosition() {				// IDA BlockActor::BlockActor
			return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this) + 44);
		}
	};

	struct BlockSource {
		// 取方块
		Block* getBlock(const BlockPos* blkpos) {
			return SYMCALL<Block*>("?getBlock@BlockSource@@QEBAAEBVBlock@@AEBVBlockPos@@@Z", this, blkpos);
		}
		// 获取方块所处维度
		int getDimensionId() {					// IDA Dimension::onBlockChanged
			return *(int*)(*((VA*)this + 4) + 200);
		}
	};
#pragma endregion

#pragma region Form
	struct ModalFormRequestPacket {
		char filler[0x48];
	};
	//取表单ID及表单选项
	struct ModalFormResponsePacket {
		UINT getFormId() {
			return *(UINT*)((VA)this + 40);
		}
		std::string getSelectStr() {
			std::string x = *(std::string*)((VA)this + 48);
			auto l = x.length();
			if (x.c_str()[l - 1] == '\n') {
				return l > 1 ? x.substr(0, l - 1) :
					x;
			}
			return x;
		}
	};
	static unsigned getFormId()
	{
		unsigned id = time(0);
		do {
			--id;
		} while (id == 0 || fids[id]);
		fids[id] = true;
		return id;
	}
	bool DestroyForm(unsigned fid)
	{
		if (fids[fid]) {
			fids.erase(fid);
			return true;
		}
		return false;
	}
	unsigned SendForm(std::string uuid, std::string str)
	{
		unsigned fid = getFormId();
		// 此处自主创建包
		auto fr = [uuid, fid, str]() {
			Player* p = PlayerUuid[uuid];
			if (PlayerOnline[p]) {
				VA tpk;
				ModalFormRequestPacket sec;
				SYMCALL<VA>("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z", &tpk, 100);
				*(VA*)(tpk + 40) = fid;
				*(std::string*)(tpk + 48) = str;
				p->sendPacket(tpk);
			}
		};
		return fid;
	}
#pragma endregion

#pragma region Command
	static VA p_spscqueue;
	struct CommandRequestPacket {
		char filler[0x90];
		// 取命令文本
		std::string toString() {			// IDA ServerNetworkHandler::handle
			std::string str = std::string(*(std::string*)((VA)this + 40));
			return str;
		}
	};
	bool runcmd(std::string cmd)
	{
		if (p_spscqueue != 0)
			return SYMCALL<bool>("??$inner_enqueue@$0A@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@?$SPSCQueue@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@$0CAA@@@AEAA_NAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z", p_spscqueue, cmd);
		return false;
	}
	bool sendText(std::string playername, std::string text)
	{
		if (OnlineQuan == 0) return false;
		std::string cmd;
		if (playername == "all")
			cmd = u8"tellraw @a {\"rawtext\":[{\"text\":\"" + to_UTF8(text) + u8"\"}]}";
		else
			cmd = u8"tellraw " + playername + " {\"rawtext\":[{\"text\":\"" + to_UTF8(text) + u8"\"}]}";
		runcmd(cmd);
		return true;
	}
#pragma endregion

#pragma region Other
	// 取服务端版本
	std::string getVersion() 
	{
		std::string a;
		SYMCALL<__int64>(
			"?getGameVersionString@Common@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
			(__int64)&a);		// IDA Common::getGameVersionString
		return a;
	}
#pragma endregion
}

//#endif
