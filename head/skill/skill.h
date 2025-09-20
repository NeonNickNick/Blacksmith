#ifndef SKILL_LIBRARY_H
#define SKILL_LIBRARY_H
#include <stdbool.h>
#include "constant.h"
extern int rounds;
extern char porter[100];
extern int playerplus;
extern int robotplus;
extern bool if_plus;
typedef struct movement {
    //movement用于处理临时数值变动
    int power;
    int round; //仅当round == 0或round == -1（永久效果）时节点生效，每回合对大于0的round -= 1，round == 0生效后节点从链表中移除。
    int effect; //effect用于标记特殊效果
    struct movement *next; //链表结构用于处理物理伤害的后发先结机制，新的行动从链表头部插入；
} movement;
typedef struct class_Wand{
    int is_alchemy;
}class_Wand;
// 驱动器
typedef struct class_Driver{
    int timeshield;
}class_Driver;
//钢炮
typedef struct class_Canon {
    ;
}class_Canon;
//站矛
typedef struct class_Spear {
    bool secret;
    bool basui;
    bool dragon;
    bool assault;
    int store;
}class_Spear;
// 玩家
typedef struct gamer_basic{
    int healthpoints;
    int maxhealthpoints;
    int res_iron; //资源使用res类型。res类型必须由对应get类型转化；//资源获取行动首先获得get类型，get类型经过禁言判定之后才能转换为res类型；
    int res_usableiron;
    int res_space;
    int res_time;
    int res_magic;
    int res_goldiron;
    int serialnumber_class; //serialnumber_class是从0开始的连续职业编号，初始化为-1表示无职业；0法杖1驱动器2钢炮
    class_Wand *Wand; //当选定职业后再加入对应资源；
    class_Driver *Driver;
    class_Canon * Canon;
    class_Spear * Spear;
    movement *get_resource;
    movement *attack_physic; //不同伤害类型分开结算；
    movement *attack_magic;
    movement *attack_real;
    movement *shield_common; //不同类型分开结算，优先结算真实防御类型以应对穿甲弹；
    movement *shield_thorn;
    movement *shield_real;
    int shield_common_static;
    int shield_real_static;
    movement *recovery;
    movement *effects;
    int invincible;
    int muteimmune;
    int delay;
    bool trans;
    bool if_muted;
    bool if_pierced;
} gamer_basic;
extern gamer_basic player;
extern gamer_basic robot;
void clearmovement(movement** move);
void initgamer(void);
void linkmovement(int power, int node, int effect, movement **move);
bool iron_0(gamer_basic *gamer, int n, int dec);
bool sting_1(gamer_basic *gamer, int n, int dec);
bool drill_2(gamer_basic *gamer, int n, int dec);
bool slash_3(gamer_basic *gamer, int n, int dec);
bool shield_4(gamer_basic *gamer, int extra, int dec);
bool thorn_shield_5(gamer_basic *gamer, int extra, int dec);
bool recovery_6(gamer_basic *gamer, int n, int dec);
bool half_recovery_7(gamer_basic *gamer, int n, int dec);
bool space_8(gamer_basic *gamer, int n, int dec);
bool time_9(gamer_basic *gamer, int n, int dec);
bool tear_10(gamer_basic *gamer, int n, int dec);
bool delay_protection_11(gamer_basic *gamer, int n, int dec);
bool transmit_12(gamer_basic *gamer, int n, int dec);
bool reincarnation_13(gamer_basic *gamer, int n, int dec);
bool tschaos_14(gamer_basic *gamer, int n, int dec);
bool armor_15(gamer_basic *gamer, int n, int dec);
bool wand_16(gamer_basic *gamer, int n, int dec);
bool magic_17(gamer_basic *gamer, int n, int dec);
bool magic_attack_18(gamer_basic *gamer, int n, int dec);
bool magic_shield_19(gamer_basic *gamer, int n, int dec);
bool magic_armor_20(gamer_basic *gamer, int n, int dec);
bool sacrifice_21(gamer_basic *gamer, int n, int dec);
bool mute_22(gamer_basic *gamer, int n, int dec);
bool alchemy_23(gamer_basic *gamer, int n, int dec);
bool midas_touch_24(gamer_basic *gamer, int, int dec);
bool driver_25(gamer_basic *gamer, int n, int dec);
bool space_impact_26(gamer_basic *gamer, int n, int dec);
bool time_shield_27(gamer_basic *gamer, int n, int dec);
bool tsimpact_28(gamer_basic *gamer, int n, int dec);
bool time_cov_space_29(gamer_basic *gamer, int n, int dec);
bool space_cov_time_30(gamer_basic *gamer, int n, int dec);
bool space_barrier_31(gamer_basic *gamer, int n, int dec);
bool time_fluctuation_32(gamer_basic *gamer, int n, int dec);
bool canon_33(gamer_basic *gamer, int n, int dec);
bool shell_34(gamer_basic *gamer, int n, int dec);
bool double_shell_35(gamer_basic *gamer, int n, int dec);
bool triple_shell_36(gamer_basic *gamer, int n, int dec);
bool pierce_37(gamer_basic *gamer, int n, int dec);
bool empty_38(gamer_basic *gamer, int n, int dec);
typedef struct command_skill {
    int is_valid;
    bool (*funcskill)(gamer_basic *, int n, int dec);
} command_skill;
extern command_skill blacksmith_skill[SCROLL_BUTTON_COUNT];
void declareskill();
#endif //SKILL_LIBRARY_H
