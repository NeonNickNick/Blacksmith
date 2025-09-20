#include "skill.h"
#include "connect.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
gamer_basic player;
gamer_basic robot;
void clearmovement(movement **move) {
    movement *temp = *move;
    movement *remove = *move;
    while (temp != NULL) {
        temp = temp->next;
        free(remove);
        remove = temp;
    }
    *move = NULL;
}
int rounds = 0;
char porter[100];
int playerplus = 0;
int robotplus = 0;
bool if_plus = false;
void initgamer() {
    if_plus = false;
    playerplus = 0;
    robotplus = 0;
    playercommand = -1;
    robotcommand = -1;
    porter[0] = '\0';
    port[0] = '\0';
    portsite = 0;
    rounds = 1;
    player.healthpoints = 10;
    player.maxhealthpoints = 10;
    player.res_iron = 0;
    player.res_usableiron = 0;
    player.res_space = 0;
    player.res_time = 0;
    player.res_magic = 0;
    player.res_goldiron = 0;
    player.serialnumber_class = -1; //serialnumber_class是从0开始的连续职业编号，初始化为-1表示无职业；
    free(player.Wand);
    player.Wand = NULL; //当选定职业后再加入对应资源；
    free(player.Driver);
    player.Driver = NULL;
    free(player.Canon);
    player.Canon = NULL;
    free(player.Spear);
    player.Spear = NULL;
    clearmovement(&player.get_resource);
    clearmovement(&player.attack_physic); //不同伤害类型分开结算；
    clearmovement(&player.attack_magic);
    clearmovement(&player.attack_real);
    clearmovement(&player.shield_common); //不同类型分开结算，优先结算真实防御类型以应对穿甲弹；
    clearmovement(&player.shield_thorn);
    clearmovement(&player.shield_real);
    player.shield_common_static = 0;
    player.shield_real_static = 0;
    clearmovement(&player.recovery);
    clearmovement(&player.effects);
    player.invincible = 0;
    player.muteimmune = 0;
    player.delay = 0;
    player.trans = false;
    player.if_muted = false;
    player.if_pierced = false;
    robot.healthpoints = 10;
    robot.maxhealthpoints = 10;
    robot.res_iron = 0;
    robot.res_usableiron = 0;
    robot.res_space = 0;
    robot.res_time = 0;
    robot.res_magic = 0;
    robot.res_goldiron = 0;
    robot.serialnumber_class = -1; //serialnumber_class是从0开始的连续职业编号，初始化为-1表示无职业；
    free(robot.Wand);
    robot.Wand = NULL; //当选定职业后再加入对应资源；
    free(robot.Driver);
    robot.Driver = NULL;
    free(robot.Canon);
    robot.Canon = NULL;
    free(robot.Spear);
    robot.Spear = NULL;
    clearmovement(&robot.get_resource);
    clearmovement(&robot.attack_physic); //不同伤害类型分开结算；
    clearmovement(&robot.attack_magic);
    clearmovement(&robot.attack_real);
    clearmovement(&robot.shield_common); //不同类型分开结算，优先结算真实防御类型以应对穿甲弹；
    clearmovement(&robot.shield_thorn);
    clearmovement(&robot.shield_real);
    robot.shield_common_static = 0;
    robot.shield_real_static = 0;
    clearmovement(&robot.recovery);
    clearmovement(&robot.effects);
    robot.invincible = 0;
    robot.muteimmune = 0;
    robot.delay = 0;
    robot.trans = false;
    robot.if_muted = false;
    robot.if_pierced = false;
}
void linkmovement(int power, int node, int effect, movement **move) {
    movement *helper = (movement *) malloc(sizeof(movement));
    helper->power = power;
    helper->round = node;
    helper->effect = effect;
    helper->next = *move;
    *move = helper;
    helper = NULL;
}

bool iron_0(gamer_basic *gamer, int n, int dec) {
    if (dec == 0) {
        return true;
    }
    linkmovement(2, 0, -1, &(gamer->get_resource));
    return true;
} //打铁
bool sting_1(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 1;
    linkmovement(1, 0, -1, &(gamer->attack_physic));
    return true;
} //刺
bool drill_2(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 3) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 3;
    linkmovement(3, 0, -1, &(gamer->attack_physic));
    return true;
} //钻
bool slash_3(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 5) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 5;
    linkmovement(5, 0, -1, &(gamer->attack_physic));
    return true;
} //切
bool shield_4(gamer_basic *gamer, int extra, int dec) {
    if (gamer->res_usableiron < extra) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= extra;
    linkmovement(2 + extra, 0, -1, &(gamer->shield_common));
    return true;
} //盾
bool thorn_shield_5(gamer_basic *gamer, int extra, int dec) {
    if (gamer->res_usableiron < 2 + extra) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= (2 + extra);
    linkmovement(4 + extra, 0, 2, &(gamer->shield_thorn));
    return true;
} //刺盾
bool recovery_6(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(2, -1, -1, &(gamer->get_resource));
    return true;
} //恢复
bool half_recovery_7(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 1;
    linkmovement(1, -1, -1, &(gamer->get_resource));
    return true;
} //恢复半
bool space_8(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 6) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 6;
    linkmovement(1, 3, -1, &(gamer->get_resource));
    return true;
} //空间
bool time_9(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 6 || gamer->delay != 0) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 6;
    linkmovement(1, 4, -1, &(gamer->get_resource));
    return true;
} //时间
bool tear_10(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_space < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_space -= 1;
    linkmovement(8, 0, -1, &(gamer->attack_physic));
    return true;
} //撕裂
bool delay_protection_11(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_time < 1 || gamer->delay != 0) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_time -= 1;
    gamer->delay = 3;
    return true;
} //延时保护
bool transmit_12(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_space < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_space -= 2;
    gamer->trans = true;
    return true;
} //转移
bool reincarnation_13(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_time < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_time -= 2;
    gamer->invincible = 1;
    gamer->healthpoints = 10;
    gamer->maxhealthpoints = 10;
    gamer->muteimmune = 0;
    gamer->serialnumber_class = -1;
    return true;
} //轮回
bool tschaos_14(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_space < 1 || gamer->res_time < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_space -= 1;
    gamer->res_time -= 1;
    linkmovement(8, 0, -1, &(gamer->attack_physic));
    gamer->shield_real_static += 6;
    return true;
} //时空混乱
bool armor_15(gamer_basic *gamer, int n, int dec) {
    if (gamer->res_usableiron < 14) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 14;
    gamer->shield_common_static += 12;
    return true;
} //装甲
bool wand_16(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != -1 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    gamer->serialnumber_class = 0;
    free(gamer->Wand);
    gamer->Wand = NULL;
    gamer->Wand = (class_Wand *) malloc(sizeof(class_Wand));
    gamer->Wand->is_alchemy = 0;
    return true;
} //法杖
bool magic_17(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(1, 1, -1, &(gamer->get_resource));
    return true;
} //积魔
bool magic_attack_18(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->res_magic < n + 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_magic -= (n + 1);
    linkmovement(2 * (n + 1), 0, -1, &(gamer->attack_magic));
    linkmovement(2 * (n + 1), 2, -1, &(gamer->attack_magic));
    linkmovement(2 * (n + 1), 4, -1, &(gamer->attack_magic));
    return true;
} //魔法
bool magic_shield_19(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->res_magic < n + 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_magic -= (n + 1);
    linkmovement(3 * (n + 1), 0, -1, &(gamer->shield_real));
    linkmovement(3 * (n + 1), 1, -1, &(gamer->shield_real));
    linkmovement(3 * (n + 1), 2, -1, &(gamer->shield_real));
    return true;
} //魔盾
bool magic_armor_20(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->res_magic < n + 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_magic -= (n + 1);
    gamer->shield_common_static += 3 * (n + 1);
    return true;
} //魔甲
bool sacrifice_21(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->healthpoints < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->healthpoints -= 1;
    gamer->maxhealthpoints -= 1;
    linkmovement(3, 0, -1, &(gamer->get_resource));
    linkmovement(7, 0, -1, &(gamer->shield_real));
    return true;
} //献祭
bool mute_22(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    linkmovement(0, 0, 1, &(gamer->effects));
    return true;
} //禁言
bool alchemy_23(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->res_usableiron < 5 || gamer->Wand->is_alchemy) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 5;
    gamer->Wand->is_alchemy = 1;
    return true;
} //炼金术
bool midas_touch_24(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 0 || gamer->Wand->is_alchemy == 0 || gamer->res_iron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_iron -= 2;
    gamer->res_usableiron -= 2;
    linkmovement(10, 2, -1, &(gamer->get_resource));
    return true;
} //点铁成金
bool driver_25(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != -1 || gamer->res_usableiron < 6) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 6;
    gamer->serialnumber_class = 1;
    free(gamer->Driver);
    gamer->Driver = NULL;
    gamer->Driver = (class_Driver *) malloc(sizeof(class_Driver));
    gamer->Driver->timeshield = 1;
    return true;
} //驱动器
bool space_impact_26(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_space < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_space -= 1;
    linkmovement(12, 0, -1, &gamer->attack_physic);
    return true;
} //空间冲击
bool time_shield_27(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_time < 1 || gamer->Driver->timeshield >= 7) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_time -= 1;
    gamer->Driver->timeshield += 3;
    return true;
} //时之盾
bool tsimpact_28(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_time < 1 || gamer->res_space < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_time -= 1;
    gamer->res_space -= 1;
    linkmovement(12, 0, -1, &(gamer->attack_physic));
    linkmovement(6, 0, -1, &(gamer->shield_real));
    return true;
} //时空冲击
bool time_cov_space_29(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_time < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_time -= 1;
    linkmovement(1, 3, -1, &(gamer->get_resource));
    linkmovement(3, 0, -1, &(gamer->shield_real));
    return true;
} //时->空
bool space_cov_time_30(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_space < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    linkmovement(1, 4, -1, &(gamer->get_resource));
    linkmovement(3, 0, -1, &(gamer->shield_real));
    return true;
} //空->时
bool space_barrier_31(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_space < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    linkmovement(5 + gamer->res_space + gamer->res_time, 0, -1, &(gamer->shield_real));
    return true;
} //空间屏障
bool time_fluctuation_32(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 1 || gamer->res_time < 1) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->invincible = 1;
    gamer->muteimmune = 5;
    return true;
} //时间波动
bool canon_33(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != -1 || gamer->res_usableiron < 6) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 6;
    gamer->serialnumber_class = 2;
    free(gamer->Canon);
    gamer->Canon = NULL;
    gamer->Canon = (class_Canon *) malloc(sizeof(class_Canon));
    linkmovement(3, 0, -1, &(gamer->shield_real));
    linkmovement(3, 1, -1, &(gamer->shield_real));
    linkmovement(3, 2, -1, &(gamer->shield_real));
    return true;
}//钢炮
bool shell_34(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 2 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(3, 0, -1, &(gamer->attack_physic));
    return true;
}//炮击
bool double_shell_35(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 2 || gamer->res_usableiron < 4) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(7, 0, -1, &(gamer->attack_physic));
    return true;
}//二连击
bool triple_shell_36(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 2 || gamer->res_usableiron < 6) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(11, 0, -1, &(gamer->attack_physic));
    return true;
}//三连击
bool pierce_37(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 2 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    linkmovement(2, 0, 0, &(gamer->attack_physic));
    return true;
}
bool spear_38(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != -1 || gamer->res_usableiron < 6) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 6;
    gamer->serialnumber_class = 3;
    free(gamer->Spear);
    gamer->Spear = NULL;
    gamer->Spear = (class_Spear *) malloc(sizeof(class_Spear));
    gamer->Spear->secret = false;
    gamer->Spear->basui = false;
    gamer->Spear->dragon = false;
    gamer->Spear->assault;
    gamer->Spear->store = 0;
    return true;
} //战矛
void spearhelper(gamer_basic* gamer, int* times) {
    gamer->Spear->store = 0;
    if(gamer->Spear->secret) {
        gamer->Spear->secret = false;
        *times = 2;
    }
    if(gamer->Spear->basui) {
        gamer->Spear->basui = false;
        gamer->healthpoints = gamer->maxhealthpoints;
    }
    if(gamer->Spear->dragon) {
        gamer->Spear->dragon = false;
        gamer->shield_common_static += 3;
    }
    if(gamer->Spear->assault) {
        gamer->Spear->assault = false;
        linkmovement(1, 0, -1, &(gamer->attack_real));
        linkmovement(1, 2, -1, &(gamer->attack_real));
    }
}
bool secret_39(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    int times = 1;
    spearhelper(gamer, &times);
    linkmovement(3 * times, 0, 3, &(gamer->attack_physic));
    return true;
}//天机
bool basui_40(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    int times = 1;
    spearhelper(gamer, &times);
    linkmovement(3 * times, 0, 4, &(gamer->attack_physic));
    return true;
}//霸碎
bool dragon_41(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    int times = 1;
    spearhelper(gamer, &times);
    linkmovement(3 * times, 0, 5, &(gamer->attack_physic));
    linkmovement(3, 0, -1, &(gamer->shield_real));
    return true;
}//龙牙
bool assault_42(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->res_usableiron < 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 2;
    int times = 1;
    spearhelper(gamer, &times);
    linkmovement(2 * times, 0, 6, &(gamer->attack_physic));
    linkmovement(2 * times, 0, 6, &(gamer->attack_physic));
    linkmovement(1 * times, 0, 6, &(gamer->attack_physic));
    return true;
}//连突
bool store_43(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->Spear->store == 2) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    ++gamer->Spear->store;
    return true;
}//蓄力
bool fulongxiangtian_44(gamer_basic *gamer, int n, int dec) {
    if (gamer->serialnumber_class != 3 || gamer->res_usableiron < 8) {
        return false;
    }
    if (dec == 0) {
        return true;
    }
    gamer->res_usableiron -= 8;
    int times = 1;
    spearhelper(gamer, &times);
    linkmovement((10 + 4 * gamer->Spear->store) * times, 0, 6, &(gamer->attack_magic));
    return true;
}//伏龙翔天
bool empty_45(gamer_basic *gamer, int n, int dec) {
    return false;
}
//将所有资源从-1开始进行编号：-1生命0铁1魔2金铁3空间4时间...
//将所有特殊效果从-1开始编号：-1无效果0破甲(3倍)1禁言2反伤3天机4霸碎5龙牙6连突
command_skill blacksmith_skill[SCROLL_BUTTON_COUNT];
void declareskill() {
    blacksmith_skill[0].funcskill = iron_0;
    blacksmith_skill[1].funcskill = sting_1;
    blacksmith_skill[2].funcskill = drill_2;
    blacksmith_skill[3].funcskill = slash_3;
    blacksmith_skill[4].funcskill = shield_4;
    blacksmith_skill[5].funcskill = thorn_shield_5;
    blacksmith_skill[6].funcskill = recovery_6;
    blacksmith_skill[7].funcskill = half_recovery_7;
    blacksmith_skill[8].funcskill = space_8;
    blacksmith_skill[9].funcskill = time_9;
    blacksmith_skill[10].funcskill = tear_10;
    blacksmith_skill[11].funcskill = delay_protection_11;
    blacksmith_skill[12].funcskill = transmit_12;
    blacksmith_skill[13].funcskill = reincarnation_13;
    blacksmith_skill[14].funcskill = tschaos_14;
    blacksmith_skill[15].funcskill = armor_15;
    blacksmith_skill[16].funcskill = wand_16;
    blacksmith_skill[17].funcskill = magic_17;
    blacksmith_skill[18].funcskill = magic_attack_18;
    blacksmith_skill[19].funcskill = magic_shield_19;
    blacksmith_skill[20].funcskill = magic_armor_20;
    blacksmith_skill[21].funcskill = sacrifice_21;
    blacksmith_skill[22].funcskill = mute_22;
    blacksmith_skill[23].funcskill = alchemy_23;
    blacksmith_skill[24].funcskill = midas_touch_24;
    blacksmith_skill[25].funcskill = driver_25;
    blacksmith_skill[26].funcskill = space_impact_26;
    blacksmith_skill[27].funcskill = time_shield_27;
    blacksmith_skill[28].funcskill = tsimpact_28;
    blacksmith_skill[29].funcskill = time_cov_space_29;
    blacksmith_skill[30].funcskill = space_cov_time_30;
    blacksmith_skill[31].funcskill = space_barrier_31;
    blacksmith_skill[32].funcskill = time_fluctuation_32;
    blacksmith_skill[33].funcskill = canon_33;
    blacksmith_skill[34].funcskill = shell_34;
    blacksmith_skill[35].funcskill = double_shell_35;
    blacksmith_skill[36].funcskill = triple_shell_36;
    blacksmith_skill[37].funcskill = pierce_37;
    blacksmith_skill[38].funcskill = spear_38;
    blacksmith_skill[39].funcskill = secret_39;
    blacksmith_skill[40].funcskill = basui_40;
    blacksmith_skill[41].funcskill = dragon_41;
    blacksmith_skill[42].funcskill = assault_42;
    blacksmith_skill[43].funcskill = store_43;
    blacksmith_skill[44].funcskill = fulongxiangtian_44;
    blacksmith_skill[45].funcskill = empty_45;
}