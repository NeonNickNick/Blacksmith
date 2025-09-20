#include "text.h"
char name_skill[SCROLL_BUTTON_COUNT][50] = {
    "Iron", "Sting", "Drill", "Slash", "Shield", "Thorn Shield", "Recovery", "Half Recovery", "Space",

    "Time", "Tear", "Delay Protection", "Transmit", "Reincarnation", "T&S Chaos", "Armor",

    "Wand", "Magic", "Magic Attack", "Magic Shield", "Magic Armor", "Sacrifice", "Mute", "Alchemy Book", "Midas Touch",

    "Driver", "Space Impact", "Time Shield", "T&S Impact", "Time cov Space", "Space cov Time", "Space Barrier",
    "Time Fluctuation",

    "Canon", "Shell", "Double Shell", "Triple Shell", "Pierce",

    "Spear", "Flame", "Light", "Ice", "Darkness", "Charge", "Dragon",

    " "
};
char name_skill_explanation[SCROLL_BUTTON_COUNT][250] = {
    "Iron\n\nNeeds: Nothing.\neffect: Get 1iron.\nTips: It can be interrupted.",
    "Sting\n\nNeeds: 1 iron.\neffect: 1 PATK.\nTips: Nothing.",
    "Drill\n\nNeeds: 3 iron.\neffect: 3 PATK.\nTips: Nothing.",
    "Slash\n\nNeeds: 5 iron.\neffect: 5 PATK.\nTips: Nothing.",
    "Shield\n\nNeeds: n iron.\neffect: Get 2+n DEF only in this round.\nTips: n must be natural number.",
    "Thorn Shield\n\nNeeds: 2+n iron.\neffect: Get 4+n DEF only in this round.\nTips: n must be natural number.50% of the PATK it blocks will reflected back to the opponent as MATK in the next round.",
    "Recovery\n\nNeeds: 2 iron.\neffect: Get 2 HP.\nTips: It can be interrupted.",
    "Half Recovery\n\nNeeds: 1 iron.\neffect: Get 1 HP.\nTips: It can be interrupted.",
    "Space\n\nNeeds: 6 iron.\neffect: Get 1 space.\nTips: It can be muted.",
    "Time\n\nNeeds: 6 iron.\neffect: Get 1 time.\nTips: It can be muted.",
    "Tear\n\nNeeds: 1 space.\neffect: 8 PATK.\nTips: Nothing.",
    "Delay Protection\n\nNeeds: 1 time.\neffect: In the next three turns, you cannot use any times, and all the damage and effects you receive will be delayed and take effect three turns later.\nTips: Nothing.",
    "Transmit\n\nNeeds: 2 space.\neffect: All the damage and effects you receive this turn will be transferred to your opponent in the next turn.\nTips: Nothing.",
    "Reincarnation\n\nNeeds: 2 time.\neffect: Become invincible in this round, reset your MHP and HP to 10 points, clear all of your effects and lose all class equipment.\nTips: Nothing.",
    "T&S Chaos\n\nNeeds: 1 space & 1 time.\neffect: 8 PATK & 6 RDEF only in this round.\nTips: Nothing.",
    "Armor\n\nNeeds: 14 iron.\neffect: Get 12 DEF for permanent. It can be damaged, with the damage points equal to the amount of damage it blocked.\nTips: Nothing.",
    "Wand\n\nNeeds: 2 iron.\neffect: Get a Wand.\nTips: Nothing.",
    "Magic\n\nNeeds: 2 iron.\neffect: Get 1 magic.\nTips: You must already have a Wand.\nIt can be interrupted.",
    "Magic Attack\n\nNeeds: n magic.\neffect: 2n MATK for 3 rounds.\nTips: You must already have a Wand.\nn must be a positive integer.",
    "Magic Shield\n\nNeeds: n magic.\neffect: 3n RDEF for 3 rounds.\nTips: You must already have a Wand.\nn must be a positive integer.",
    "Magic Armor\n\nNeeds: n magic.\neffect: 3n DEF for permanet. It can be damaged, with the damage points equal to the amount of damage it blocked.\nTips: You must already have a Wand.\nn must be a positive integer.",
    "Sacrifice\n\nNeeds: 1 HP & 1 MHP.\neffect: Get 3 iron and 7 RDEF only in this round.\nTips: You must already have a Wand.",
    "Mute\n\nNeeds: Nothing.\neffect: Your opponent cannot get any space or time in this round.\nTips: You must already have a Wand.\nIt can be immune.",
    "Alchemy Book\n\nNeeds: 5 iron.\neffect: Get an Alchemy Book.\nTips: You must already have a Wand.",
    "Midas Touch\n\nNeeds: 2 iron.\neffect: Get 10 goldiron.\nTips: You must already have an Alchemy Book. Goldiron can be used as iron, but Midas Touch does not work on it.",
    "Driver\n\nNeeds: 6 iron.\neffect: Get a Driver with 1 RDEF for permanent.\nTips: Nothing.",
    "Space Impact\n\nNeeds: 1 space.\neffect: 12 PATK.\nTips: You must already have a Driver.",
    "Time Shield\n\nNeeds: 1 time.\neffect: Get 3RDEF for permanent.\nTips: You must already have a Driver.",
    "T&S Impact\n\nNeeds: 1 space & 1 time.\neffect: 12 PATK & 6 RDEF only in this round.\nTips: You must already have a Driver.",
    "Space cov Time\n\nNeeds: 1 space.\neffect: Get 1 time and 3 RDEF only in this round.\nTips: You must already have a Driver.",
    "Time cov Space\n\nNeeds: 1 time.\neffect: Get 1 space and 3 RDEF only in this round.\nTips: You must already have a Driver.",
    "Space Barrier\n\nNeeds: 2 iron.\neffect: Get (5 + amount of your space + amount of your time) RDEF only in this round.\nTips: You must already have a Driver.",
    "Time Fluctuation\n\nNeeds: 1 time.\neffect: Become invincible in this round and immune Mute in the next four rounds.\nTips: You must already have a Driver.",
    "Canon\n\nNeeds: 6 iron.\neffect: Get a Canon with 3 RDEF for 3 rounds.\nTips: Nothing.",
    "Shell\n\nNeeds: 2 iron.\neffect: 3 PATK.\nTips: Nothing.",
    "Double Shell\n\nNeeds: 4 iron.\neffect: 7 PATK.\nTips: Nothing.",
    "Triple Shell\n\nNeeds: 6 iron.\neffect: 11 PATK.\nTips: Nothing.",
    "Pierce\n\nNeeds: 2 iron.\neffect: 2 PATK with piercing effect * 3.\nTips: Nothing.",
    "Spear\n\nNeeds: 6 iron.\neffect: Get a Spear.\nTips: Nothing.",
    "Flame\n\nNeeds: 2 iron.\neffect: 3 PATK. If causes damage, doubles the power next time you take action.\nTips: Nothing.",
    "Light\n\nNeeds: 2 iron.\neffect: 3 PATK with piercing effect * 2. If causes damage, restores you healthpoints to the upper limit next time you take action.\nTips: Nothing.",
    "Ice\n\nNeeds: 2 iron.\neffect: 3 PATK with 3 RDEF only in  this round. If causes damage, you will get 3 DEF for permanent next time you take action.\nTips: Nothing.",
    "Darkness\n\nNeeds: 2 iron.\neffect: 2 + 2 + 1 PATK. If causes damage, get additional 1 RATK for 2 rounds next time you take action.\nTips: Nothing.",
    "Charge\n\nNeeds: Nothing.\neffect: The power of Dragon += 4.\nTips: More than two consecutive uses are not allowed. If you do not continue to charge or use Dragon after charging, you will lose all charging effects. If you are about to take damage during the charging round, Dragon will be automatically used instead of Charge.",
    "Dragon\n\nNeeds: 4 iron.\neffect: 10 + 4 * charging times MATK.\nTips: Nothing.",
    "  "
};
// 帮助文字
const char *helpText =
        "Welcome to the epic strategy game Blacksmith in the new era! In this game, you can engage in battles between legendary heroes with blacksmiths online. Hone your skills through continuous competition and version updates while learning knowledge in fields like psychology, game theory, limits, expectations, and more. "
        "In the world of blacksmithing, there are three main factions: the Empire, the Tribe, and the Federation. Each faction has its own unique characteristics: the Empire masters magical techniques and exquisite means of governance; the Tribe, living in the wilderness, commands ancient powers strong enough to survive the conflict between the Empire and the Federation; the Federation is an alliance of regions with a separation of powers, wielding intelligent technological strength and a firm hand against dissenting forces. "
        "Although the conflict between the Empire and the Federation has ceased for nearly a century, the stories of the heroes who emerged during the war have been passed down by blacksmiths for generations. The clever blacksmiths created this strategy game to recreate the battles of the heroes, pass the time, and sharpen their minds. "
        "Since you're eager to give it a try, there's nothing more to say—come and have a seat at the Blacksmith Tavern in North Brack!"
        "\n\nGame Instructions:\nUse scroll wheel to navigate. Click the left mouse button to select an action, and right-click to view the description. Once you have made a decision, press SPACE."
        "\n\nImportant Terms:\n"
        "HP: Your healthpoints. If your HP = 0, you lose.\n"
        "MHP: The maximum if your HP.\n"
        "PATK: Physical attack from the source, which can be reflected to its source.\n"
        "MATK: Magic attack from no source, which cannot be reflected to its source.\n"
        "RATK: Real attack from no source, which cannot be reflected to its source or blocked by any                kinds of DEF.\n"
        "DEF & RDEF: Both DEF & RDEF can reduce the damage you take. Attacks with armor-piercing                           effects deal significant additional damage to DEF, while RDEF always reduces                             damage by an amount equals to its value.\n"
        "\nGame mechanics:\n"
        "1. You must take an action every turn, and you can only act once per turn.\n"
        "2. You may choose a class equipment. Once you make your choice, it is not allowed to change unless you use Reincarnation.\n"
        "3. PATK cancels out with PATK first, MATK cancels with out MATK first, then PATK cancels out with MATK. RATK does not cancel out with any kinds of ATK.\n"
        "4. There are two types of DEF & RDEF: temporary and permanent. Permanent DEF can be damaged, but permanent RDEF cannot. "
        "PATK & MATK must pass through DEF before reaching you. RDEF is on the outermost layer, DEF with reflect damage effects is in the middle, temporary DEF is inside, and permanent DEF is the innermost layer.\n"
        "5. If you are using Recovery or already have a health regeneration effect this turn, you will always restore healthpoints before the ATK arrives.\n\n"
        "Backgroung music: Ludwig van Beethoven, Piano Sonata No.32, Op.111, Movement 2, \n                             performed by Penelope Crawford.\n\n"
        "Thanks for: H, W, S, L.\n\n"
        "Neonnicknick, 2025, 4, 14";

#include <stdio.h>

