/*
* ---------------------------------------------------------------------------
* Title         :   Transaction Mnemonics Header File (Prog. 20)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P020.H
* Copyright (C) :   1995, NCR Corp. E&M-OISO, All rights reserved.
* ---------------------------------------------------------------------------
* Compiler      :   MS-C Ver. 7.00 by Microsoft Corp.
* Memory Model  :   Large Model
* Options       :   /AL /W4 /G2 /GEf /GA /Zp /f- /Os /Og /Oe /Gs
* ---------------------------------------------------------------------------
* Abstract :
*
* ---------------------------------------------------------------------------
* Update Histories
*   Date    : Ver.Rev. :   Name     : Description
* Dec-22-93 : 00.00.01 : M.Yamamoto : Initial (Migrate from HP US Model)
* Apr-21-95 : 03.00.00 : H.Ishida   : Add Combination Coupon
* Jan-24-96 : 03.00.06 : M.Suzuki   : Add R3.1
* Sep-06-98 : 03.03.00 : A.Mitsui   : Add V3.3
*
* ===========================================================================
* ===========================================================================
* PVCS Entry
* ---------------------------------------------------------------------------
* $Revision$
* $Date$
* $Author$
* $Log$
* ===========================================================================
*/
/*
* ===========================================================================
*       Define Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Dialog ID 
* ===========================================================================
*/
#define IDD_P20_SCBAR   22001

#define IDD_P20_DESC01  (IDD_P20_SCBAR +  1)
#define IDD_P20_DESC02  (IDD_P20_SCBAR +  2)
#define IDD_P20_DESC03  (IDD_P20_SCBAR +  3)
#define IDD_P20_DESC04  (IDD_P20_SCBAR +  4)
#define IDD_P20_DESC05  (IDD_P20_SCBAR +  5)
#define IDD_P20_DESC06  (IDD_P20_SCBAR +  6)
#define IDD_P20_DESC07  (IDD_P20_SCBAR +  7)
#define IDD_P20_DESC08  (IDD_P20_SCBAR +  8)
#define IDD_P20_DESC09  (IDD_P20_SCBAR +  9)
#define IDD_P20_DESC10  (IDD_P20_SCBAR + 10)
#define IDD_P20_DESC11  (IDD_P20_SCBAR + 11)
#define IDD_P20_DESC12  (IDD_P20_SCBAR + 12)
#define IDD_P20_DESC13  (IDD_P20_SCBAR + 13)
#define IDD_P20_DESC14  (IDD_P20_SCBAR + 14)
#define IDD_P20_DESC15  (IDD_P20_SCBAR + 15)
#define IDD_P20_DESC16  (IDD_P20_SCBAR + 16)
#define IDD_P20_DESC17  (IDD_P20_SCBAR + 17)
#define IDD_P20_DESC18  (IDD_P20_SCBAR + 18)
#define IDD_P20_DESC19  (IDD_P20_SCBAR + 19)
#define IDD_P20_DESC20  (IDD_P20_SCBAR + 20)

#define IDD_P20_EDIT01  (IDD_P20_SCBAR + 21)
#define IDD_P20_EDIT02  (IDD_P20_SCBAR + 22)
#define IDD_P20_EDIT03  (IDD_P20_SCBAR + 23)
#define IDD_P20_EDIT04  (IDD_P20_SCBAR + 24)
#define IDD_P20_EDIT05  (IDD_P20_SCBAR + 25)
#define IDD_P20_EDIT06  (IDD_P20_SCBAR + 26)
#define IDD_P20_EDIT07  (IDD_P20_SCBAR + 27)
#define IDD_P20_EDIT08  (IDD_P20_SCBAR + 28)
#define IDD_P20_EDIT09  (IDD_P20_SCBAR + 29)
#define IDD_P20_EDIT10  (IDD_P20_SCBAR + 30)
#define IDD_P20_EDIT11  (IDD_P20_SCBAR + 31)
#define IDD_P20_EDIT12  (IDD_P20_SCBAR + 32)
#define IDD_P20_EDIT13  (IDD_P20_SCBAR + 33)
#define IDD_P20_EDIT14  (IDD_P20_SCBAR + 34)
#define IDD_P20_EDIT15  (IDD_P20_SCBAR + 35)
#define IDD_P20_EDIT16  (IDD_P20_SCBAR + 36)
#define IDD_P20_EDIT17  (IDD_P20_SCBAR + 37)
#define IDD_P20_EDIT18  (IDD_P20_SCBAR + 38)
#define IDD_P20_EDIT19  (IDD_P20_SCBAR + 39)
#define IDD_P20_EDIT20  (IDD_P20_SCBAR + 40)

#define IDD_P20_ADDR	(IDD_P20_SCBAR + 41)
#define IDD_P20_DESC	(IDD_P20_SCBAR + 42)
#define IDD_P20_MNEM	(IDD_P20_SCBAR + 43)
#define IDD_P20_CAPTION	(IDD_P20_SCBAR + 44)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P020DESC001 IDS_P20                 /* Description Resouce ID   */
#define IDS_P020DESC002 (IDS_P020DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P020DESC003 (IDS_P020DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P020DESC004 (IDS_P020DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P020DESC005 (IDS_P020DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P020DESC006 (IDS_P020DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P020DESC007 (IDS_P020DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P020DESC008 (IDS_P020DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P020DESC009 (IDS_P020DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P020DESC010 (IDS_P020DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P020DESC011 (IDS_P020DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P020DESC012 (IDS_P020DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P020DESC013 (IDS_P020DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P020DESC014 (IDS_P020DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P020DESC015 (IDS_P020DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P020DESC016 (IDS_P020DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P020DESC017 (IDS_P020DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P020DESC018 (IDS_P020DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P020DESC019 (IDS_P020DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P020DESC020 (IDS_P020DESC001 + 19)  /* Description Resouce ID   */
#define IDS_P020DESC021 (IDS_P020DESC001 + 20)  /* Description Resouce ID   */
#define IDS_P020DESC022 (IDS_P020DESC001 + 21)  /* Description Resouce ID   */
#define IDS_P020DESC023 (IDS_P020DESC001 + 22)  /* Description Resouce ID   */
#define IDS_P020DESC024 (IDS_P020DESC001 + 23)  /* Description Resouce ID   */
#define IDS_P020DESC025 (IDS_P020DESC001 + 24)  /* Description Resouce ID   */
#define IDS_P020DESC026 (IDS_P020DESC001 + 25)  /* Description Resouce ID   */
#define IDS_P020DESC027 (IDS_P020DESC001 + 26)  /* Description Resouce ID   */
#define IDS_P020DESC028 (IDS_P020DESC001 + 27)  /* Description Resouce ID   */
#define IDS_P020DESC029 (IDS_P020DESC001 + 28)  /* Description Resouce ID   */
#define IDS_P020DESC030 (IDS_P020DESC001 + 29)  /* Description Resouce ID   */
#define IDS_P020DESC031 (IDS_P020DESC001 + 30)  /* Description Resouce ID   */
#define IDS_P020DESC032 (IDS_P020DESC001 + 31)  /* Description Resouce ID   */
#define IDS_P020DESC033 (IDS_P020DESC001 + 32)  /* Description Resouce ID   */
#define IDS_P020DESC034 (IDS_P020DESC001 + 33)  /* Description Resouce ID   */
#define IDS_P020DESC035 (IDS_P020DESC001 + 34)  /* Description Resouce ID   */
#define IDS_P020DESC036 (IDS_P020DESC001 + 35)  /* Description Resouce ID   */
#define IDS_P020DESC037 (IDS_P020DESC001 + 36)  /* Description Resouce ID   */
#define IDS_P020DESC038 (IDS_P020DESC001 + 37)  /* Description Resouce ID   */
#define IDS_P020DESC039 (IDS_P020DESC001 + 38)  /* Description Resouce ID   */
#define IDS_P020DESC040 (IDS_P020DESC001 + 39)  /* Description Resouce ID   */
#define IDS_P020DESC041 (IDS_P020DESC001 + 40)  /* Description Resouce ID   */
#define IDS_P020DESC042 (IDS_P020DESC001 + 41)  /* Description Resouce ID   */
#define IDS_P020DESC043 (IDS_P020DESC001 + 42)  /* Description Resouce ID   */
#define IDS_P020DESC044 (IDS_P020DESC001 + 43)  /* Description Resouce ID   */
#define IDS_P020DESC045 (IDS_P020DESC001 + 44)  /* Description Resouce ID   */
#define IDS_P020DESC046 (IDS_P020DESC001 + 45)  /* Description Resouce ID   */
#define IDS_P020DESC047 (IDS_P020DESC001 + 46)  /* Description Resouce ID   */
#define IDS_P020DESC048 (IDS_P020DESC001 + 47)  /* Description Resouce ID   */
#define IDS_P020DESC049 (IDS_P020DESC001 + 48)  /* Description Resouce ID   */
#define IDS_P020DESC050 (IDS_P020DESC001 + 49)  /* Description Resouce ID   */
#define IDS_P020DESC051 (IDS_P020DESC001 + 50)  /* Description Resouce ID   */
#define IDS_P020DESC052 (IDS_P020DESC001 + 51)  /* Description Resouce ID   */
#define IDS_P020DESC053 (IDS_P020DESC001 + 52)  /* Description Resouce ID   */
#define IDS_P020DESC054 (IDS_P020DESC001 + 53)  /* Description Resouce ID   */
#define IDS_P020DESC055 (IDS_P020DESC001 + 54)  /* Description Resouce ID   */
#define IDS_P020DESC056 (IDS_P020DESC001 + 55)  /* Description Resouce ID   */
#define IDS_P020DESC057 (IDS_P020DESC001 + 56)  /* Description Resouce ID   */
#define IDS_P020DESC058 (IDS_P020DESC001 + 57)  /* Description Resouce ID   */
#define IDS_P020DESC059 (IDS_P020DESC001 + 58)  /* Description Resouce ID   */
#define IDS_P020DESC060 (IDS_P020DESC001 + 59)  /* Description Resouce ID   */
#define IDS_P020DESC061 (IDS_P020DESC001 + 60)  /* Description Resouce ID   */
#define IDS_P020DESC062 (IDS_P020DESC001 + 61)  /* Description Resouce ID   */
#define IDS_P020DESC063 (IDS_P020DESC001 + 62)  /* Description Resouce ID   */
#define IDS_P020DESC064 (IDS_P020DESC001 + 63)  /* Description Resouce ID   */
#define IDS_P020DESC065 (IDS_P020DESC001 + 64)  /* Description Resouce ID   */
#define IDS_P020DESC066 (IDS_P020DESC001 + 65)  /* Description Resouce ID   */
#define IDS_P020DESC067 (IDS_P020DESC001 + 66)  /* Description Resouce ID   */
#define IDS_P020DESC068 (IDS_P020DESC001 + 67)  /* Description Resouce ID   */
#define IDS_P020DESC069 (IDS_P020DESC001 + 68)  /* Description Resouce ID   */
#define IDS_P020DESC070 (IDS_P020DESC001 + 69)  /* Description Resouce ID   */
#define IDS_P020DESC071 (IDS_P020DESC001 + 70)  /* Description Resouce ID   */
#define IDS_P020DESC072 (IDS_P020DESC001 + 71)  /* Description Resouce ID   */
#define IDS_P020DESC073 (IDS_P020DESC001 + 72)  /* Description Resouce ID   */
#define IDS_P020DESC074 (IDS_P020DESC001 + 73)  /* Description Resouce ID   */
#define IDS_P020DESC075 (IDS_P020DESC001 + 74)  /* Description Resouce ID   */
#define IDS_P020DESC076 (IDS_P020DESC001 + 75)  /* Description Resouce ID   */
#define IDS_P020DESC077 (IDS_P020DESC001 + 76)  /* Description Resouce ID   */
#define IDS_P020DESC078 (IDS_P020DESC001 + 77)  /* Description Resouce ID   */
#define IDS_P020DESC079 (IDS_P020DESC001 + 78)  /* Description Resouce ID   */
#define IDS_P020DESC080 (IDS_P020DESC001 + 79)  /* Description Resouce ID   */
#define IDS_P020DESC081 (IDS_P020DESC001 + 80)  /* Description Resouce ID   */
#define IDS_P020DESC082 (IDS_P020DESC001 + 81)  /* Description Resouce ID   */
#define IDS_P020DESC083 (IDS_P020DESC001 + 82)  /* Description Resouce ID   */
#define IDS_P020DESC084 (IDS_P020DESC001 + 83)  /* Description Resouce ID   */
#define IDS_P020DESC085 (IDS_P020DESC001 + 84)  /* Description Resouce ID   */
#define IDS_P020DESC086 (IDS_P020DESC001 + 85)  /* Description Resouce ID   */
#define IDS_P020DESC087 (IDS_P020DESC001 + 86)  /* Description Resouce ID   */
#define IDS_P020DESC088 (IDS_P020DESC001 + 87)  /* Description Resouce ID   */
#define IDS_P020DESC089 (IDS_P020DESC001 + 88)  /* Description Resouce ID   */
#define IDS_P020DESC090 (IDS_P020DESC001 + 89)  /* Description Resouce ID   */
#define IDS_P020DESC091 (IDS_P020DESC001 + 90)  /* Description Resouce ID   */
#define IDS_P020DESC092 (IDS_P020DESC001 + 91)  /* Description Resouce ID   */
#define IDS_P020DESC093 (IDS_P020DESC001 + 92)  /* Description Resouce ID   */
#define IDS_P020DESC094 (IDS_P020DESC001 + 93)  /* Description Resouce ID   */
#define IDS_P020DESC095 (IDS_P020DESC001 + 94)  /* Description Resouce ID   */
#define IDS_P020DESC096 (IDS_P020DESC001 + 95)  /* Description Resouce ID   */
#define IDS_P020DESC097 (IDS_P020DESC001 + 96)  /* Description Resouce ID   */
#define IDS_P020DESC098 (IDS_P020DESC001 + 97)  /* Description Resouce ID   */
#define IDS_P020DESC099 (IDS_P020DESC001 + 98)  /* Description Resouce ID   */
#define IDS_P020DESC100 (IDS_P020DESC001 + 99)  /* Description Resouce ID   */
#define IDS_P020DESC101 (IDS_P020DESC001 +100)  /* Description Resouce ID   */
#define IDS_P020DESC102 (IDS_P020DESC001 +101)  /* Description Resouce ID   */
#define IDS_P020DESC103 (IDS_P020DESC001 +102)  /* Description Resouce ID   */
#define IDS_P020DESC104 (IDS_P020DESC001 +103)  /* Description Resouce ID   */
#define IDS_P020DESC105 (IDS_P020DESC001 +104)  /* Description Resouce ID   */
#define IDS_P020DESC106 (IDS_P020DESC001 +105)  /* Description Resouce ID   */
#define IDS_P020DESC107 (IDS_P020DESC001 +106)  /* Description Resouce ID   */
#define IDS_P020DESC108 (IDS_P020DESC001 +107)  /* Description Resouce ID   */
#define IDS_P020DESC109 (IDS_P020DESC001 +108)  /* Description Resouce ID   */
#define IDS_P020DESC110 (IDS_P020DESC001 +109)  /* Description Resouce ID   */
#define IDS_P020DESC111 (IDS_P020DESC001 +110)  /* Description Resouce ID   */
#define IDS_P020DESC112 (IDS_P020DESC001 +111)  /* Description Resouce ID   */
#define IDS_P020DESC113 (IDS_P020DESC001 +112)  /* Description Resouce ID   */
#define IDS_P020DESC114 (IDS_P020DESC001 +113)  /* Description Resouce ID   */
#define IDS_P020DESC115 (IDS_P020DESC001 +114)  /* Description Resouce ID   */
#define IDS_P020DESC116 (IDS_P020DESC001 +115)  /* Description Resouce ID   */
#define IDS_P020DESC117 (IDS_P020DESC001 +116)  /* Description Resouce ID   */
#define IDS_P020DESC118 (IDS_P020DESC001 +117)  /* Description Resouce ID   */
#define IDS_P020DESC119 (IDS_P020DESC001 +118)  /* Description Resouce ID   */
#define IDS_P020DESC120 (IDS_P020DESC001 +119)  /* Description Resouce ID   */
#define IDS_P020DESC121 (IDS_P020DESC001 +120)  /* Description Resouce ID   */
#define IDS_P020DESC122 (IDS_P020DESC001 +121)  /* Description Resouce ID   */
#define IDS_P020DESC123 (IDS_P020DESC001 +122)  /* Description Resouce ID   */
#define IDS_P020DESC124 (IDS_P020DESC001 +123)  /* Description Resouce ID   */
#define IDS_P020DESC125 (IDS_P020DESC001 +124)  /* Description Resouce ID   */
#define IDS_P020DESC126 (IDS_P020DESC001 +125)  /* Description Resouce ID   */
#define IDS_P020DESC127 (IDS_P020DESC001 +126)  /* Description Resouce ID   */
#define IDS_P020DESC128 (IDS_P020DESC001 +127)  /* Description Resouce ID   */
#define IDS_P020DESC129 (IDS_P020DESC001 +128)  /* Description Resouce ID   */
#define IDS_P020DESC130 (IDS_P020DESC001 +129)  /* Description Resouce ID   */
#define IDS_P020DESC131 (IDS_P020DESC001 +130)  /* Description Resouce ID   */
#define IDS_P020DESC132 (IDS_P020DESC001 +131)  /* Description Resouce ID   */

#define IDS_P020DESC133 (IDS_P020DESC001 +132)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC134 (IDS_P020DESC001 +133)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC135 (IDS_P020DESC001 +134)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC136 (IDS_P020DESC001 +135)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC137 (IDS_P020DESC001 +136)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC138 (IDS_P020DESC001 +137)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC139 (IDS_P020DESC001 +138)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC140 (IDS_P020DESC001 +139)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC141 (IDS_P020DESC001 +140)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC142 (IDS_P020DESC001 +141)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC143 (IDS_P020DESC001 +142)  /* Description Resouce ID , V3.3 */

/*** Start NCR2172 ***/
#define IDS_P020DESC144 (IDS_P020DESC001 +143)  /* Description Resouce ID   */
#define IDS_P020DESC145 (IDS_P020DESC001 +144)  /* Description Resouce ID   */
#define IDS_P020DESC146 (IDS_P020DESC001 +145)  /* Description Resouce ID   */
#define IDS_P020DESC147 (IDS_P020DESC001 +146)  /* Description Resouce ID   */
#define IDS_P020DESC148 (IDS_P020DESC001 +147)  /* Description Resouce ID   */
#define IDS_P020DESC149 (IDS_P020DESC001 +148)  /* Description Resouce ID   */
#define IDS_P020DESC150 (IDS_P020DESC001 +149)  /* Description Resouce ID   */
#define IDS_P020DESC151 (IDS_P020DESC001 +150)  /* Description Resouce ID   */
#define IDS_P020DESC152 (IDS_P020DESC001 +151)  /* Description Resouce ID   */
#define IDS_P020DESC153 (IDS_P020DESC001 +152)  /* Description Resouce ID   */
#define IDS_P020DESC154 (IDS_P020DESC001 +153)  /* Description Resouce ID   */
#define IDS_P020DESC155 (IDS_P020DESC001 +154)  /* Description Resouce ID   */
#define IDS_P020DESC156 (IDS_P020DESC001 +155)  /* Description Resouce ID   */
#define IDS_P020DESC157 (IDS_P020DESC001 +156)  /* Description Resouce ID   */
#define IDS_P020DESC158 (IDS_P020DESC001 +157)  /* Description Resouce ID   */
#define IDS_P020DESC159 (IDS_P020DESC001 +158)  /* Description Resouce ID   */
#define IDS_P020DESC160 (IDS_P020DESC001 +159)  /* Description Resouce ID   */
#define IDS_P020DESC161 (IDS_P020DESC001 +160)  /* Description Resouce ID   */
#define IDS_P020DESC162 (IDS_P020DESC001 +161)  /* Description Resouce ID   */
#define IDS_P020DESC163 (IDS_P020DESC001 +162)  /* Description Resouce ID   */
#define IDS_P020DESC164 (IDS_P020DESC001 +163)  /* Description Resouce ID   */
#define IDS_P020DESC165 (IDS_P020DESC001 +164)  /* Description Resouce ID   */
#define IDS_P020DESC166 (IDS_P020DESC001 +165)  /* Description Resouce ID   */
#define IDS_P020DESC167 (IDS_P020DESC001 +166)  /* Description Resouce ID   */
#define IDS_P020DESC168 (IDS_P020DESC001 +167)  /* Description Resouce ID   */
#define IDS_P020DESC169 (IDS_P020DESC001 +168)  /* Description Resouce ID   */
#define IDS_P020DESC170 (IDS_P020DESC001 +169)  /* Description Resouce ID   */
#define IDS_P020DESC171 (IDS_P020DESC001 +170)  /* Description Resouce ID   */
#define IDS_P020DESC172 (IDS_P020DESC001 +171)  /* Description Resouce ID   */
#define IDS_P020DESC173 (IDS_P020DESC001 +172)  /* Description Resouce ID   */
#define IDS_P020DESC174 (IDS_P020DESC001 +173)  /* Description Resouce ID   */
#define IDS_P020DESC175 (IDS_P020DESC001 +174)  /* Description Resouce ID   */
#define IDS_P020DESC176 (IDS_P020DESC001 +175)  /* Description Resouce ID   */
#define IDS_P020DESC177 (IDS_P020DESC001 +176)  /* Description Resouce ID   */
#define IDS_P020DESC178 (IDS_P020DESC001 +177)  /* Description Resouce ID   */
#define IDS_P020DESC179 (IDS_P020DESC001 +178)  /* Description Resouce ID   */
#define IDS_P020DESC180 (IDS_P020DESC001 +179)  /* Description Resouce ID   */
#define IDS_P020DESC181 (IDS_P020DESC001 +180)  /* Description Resouce ID   */
#define IDS_P020DESC182 (IDS_P020DESC001 +181)  /* Description Resouce ID   */
#define IDS_P020DESC183 (IDS_P020DESC001 +182)  /* Description Resouce ID   */
#define IDS_P020DESC184 (IDS_P020DESC001 +183)  /* Description Resouce ID   */
#define IDS_P020DESC185 (IDS_P020DESC001 +184)  /* Description Resouce ID   */
#define IDS_P020DESC186 (IDS_P020DESC001 +185)  /* Description Resouce ID   */
#define IDS_P020DESC187 (IDS_P020DESC001 +186)  /* Description Resouce ID   */
#define IDS_P020DESC188 (IDS_P020DESC001 +187)  /* Description Resouce ID   */
#define IDS_P020DESC189 (IDS_P020DESC001 +188)  /* Description Resouce ID   */
#define IDS_P020DESC190 (IDS_P020DESC001 +189)  /* Description Resouce ID   */
#define IDS_P020DESC191 (IDS_P020DESC001 +190)  /* Description Resouce ID   */
#define IDS_P020DESC192 (IDS_P020DESC001 +191)  /* Description Resouce ID   */
#define IDS_P020DESC193 (IDS_P020DESC001 +192)  /* Description Resouce ID   */
#define IDS_P020DESC194 (IDS_P020DESC001 +193)  /* Description Resouce ID   */
#define IDS_P020DESC195 (IDS_P020DESC001 +194)  /* Description Resouce ID   */
#define IDS_P020DESC196 (IDS_P020DESC001 +195)  /* Description Resouce ID   */
#define IDS_P020DESC197 (IDS_P020DESC001 +196)  /* Description Resouce ID   */
#define IDS_P020DESC198 (IDS_P020DESC001 +197)  /* Description Resouce ID   */
#define IDS_P020DESC199 (IDS_P020DESC001 +198)  /* Description Resouce ID   */
#define IDS_P020DESC200 (IDS_P020DESC001 +199)  /* Description Resouce ID   */

//BEGIN EXTRA 200 TRANSACTION MNEMONICS

#define IDS_P020DESC201 (IDS_P020DESC001 +  200)  /* Description Resouce ID   */
#define IDS_P020DESC202 (IDS_P020DESC001 +  201)  /* Description Resouce ID   */
#define IDS_P020DESC203 (IDS_P020DESC001 +  202)  /* Description Resouce ID   */
#define IDS_P020DESC204 (IDS_P020DESC001 +  203)  /* Description Resouce ID   */
#define IDS_P020DESC205 (IDS_P020DESC001 +  204)  /* Description Resouce ID   */
#define IDS_P020DESC206 (IDS_P020DESC001 +  205)  /* Description Resouce ID   */
#define IDS_P020DESC207 (IDS_P020DESC001 +  206)  /* Description Resouce ID   */
#define IDS_P020DESC208 (IDS_P020DESC001 +  207)  /* Description Resouce ID   */
#define IDS_P020DESC209 (IDS_P020DESC001 +  208)  /* Description Resouce ID   */
#define IDS_P020DESC210 (IDS_P020DESC001 +  209)  /* Description Resouce ID   */
#define IDS_P020DESC211 (IDS_P020DESC001 + 210)  /* Description Resouce ID   */
#define IDS_P020DESC212 (IDS_P020DESC001 + 211)  /* Description Resouce ID   */
#define IDS_P020DESC213 (IDS_P020DESC001 + 212)  /* Description Resouce ID   */
#define IDS_P020DESC214 (IDS_P020DESC001 + 213)  /* Description Resouce ID   */
#define IDS_P020DESC215 (IDS_P020DESC001 + 214)  /* Description Resouce ID   */
#define IDS_P020DESC216 (IDS_P020DESC001 + 215)  /* Description Resouce ID   */
#define IDS_P020DESC217 (IDS_P020DESC001 + 216)  /* Description Resouce ID   */
#define IDS_P020DESC218 (IDS_P020DESC001 + 217)  /* Description Resouce ID   */
#define IDS_P020DESC219 (IDS_P020DESC001 + 218)  /* Description Resouce ID   */
#define IDS_P020DESC220 (IDS_P020DESC001 + 219)  /* Description Resouce ID   */
#define IDS_P020DESC221 (IDS_P020DESC001 + 220)  /* Description Resouce ID   */
#define IDS_P020DESC222 (IDS_P020DESC001 + 221)  /* Description Resouce ID   */
#define IDS_P020DESC223 (IDS_P020DESC001 + 222)  /* Description Resouce ID   */
#define IDS_P020DESC224 (IDS_P020DESC001 + 223)  /* Description Resouce ID   */
#define IDS_P020DESC225 (IDS_P020DESC001 + 224)  /* Description Resouce ID   */
#define IDS_P020DESC226 (IDS_P020DESC001 + 225)  /* Description Resouce ID   */
#define IDS_P020DESC227 (IDS_P020DESC001 + 226)  /* Description Resouce ID   */
#define IDS_P020DESC228 (IDS_P020DESC001 + 227)  /* Description Resouce ID   */
#define IDS_P020DESC229 (IDS_P020DESC001 + 228)  /* Description Resouce ID   */
#define IDS_P020DESC230 (IDS_P020DESC001 + 229)  /* Description Resouce ID   */
#define IDS_P020DESC231 (IDS_P020DESC001 + 230)  /* Description Resouce ID   */
#define IDS_P020DESC232 (IDS_P020DESC001 + 231)  /* Description Resouce ID   */
#define IDS_P020DESC233 (IDS_P020DESC001 + 232)  /* Description Resouce ID   */
#define IDS_P020DESC234 (IDS_P020DESC001 + 233)  /* Description Resouce ID   */
#define IDS_P020DESC235 (IDS_P020DESC001 + 234)  /* Description Resouce ID   */
#define IDS_P020DESC236 (IDS_P020DESC001 + 235)  /* Description Resouce ID   */
#define IDS_P020DESC237 (IDS_P020DESC001 + 236)  /* Description Resouce ID   */
#define IDS_P020DESC238 (IDS_P020DESC001 + 237)  /* Description Resouce ID   */
#define IDS_P020DESC239 (IDS_P020DESC001 + 238)  /* Description Resouce ID   */
#define IDS_P020DESC240 (IDS_P020DESC001 + 239)  /* Description Resouce ID   */
#define IDS_P020DESC241 (IDS_P020DESC001 + 240)  /* Description Resouce ID   */
#define IDS_P020DESC242 (IDS_P020DESC001 + 241)  /* Description Resouce ID   */
#define IDS_P020DESC243 (IDS_P020DESC001 + 242)  /* Description Resouce ID   */
#define IDS_P020DESC244 (IDS_P020DESC001 + 243)  /* Description Resouce ID   */
#define IDS_P020DESC245 (IDS_P020DESC001 + 244)  /* Description Resouce ID   */
#define IDS_P020DESC246 (IDS_P020DESC001 + 245)  /* Description Resouce ID   */
#define IDS_P020DESC247 (IDS_P020DESC001 + 246)  /* Description Resouce ID   */
#define IDS_P020DESC248 (IDS_P020DESC001 + 247)  /* Description Resouce ID   */
#define IDS_P020DESC249 (IDS_P020DESC001 + 248)  /* Description Resouce ID   */
#define IDS_P020DESC250 (IDS_P020DESC001 + 249)  /* Description Resouce ID   */
#define IDS_P020DESC251 (IDS_P020DESC001 + 250)  /* Description Resouce ID   */
#define IDS_P020DESC252 (IDS_P020DESC001 + 251)  /* Description Resouce ID   */
#define IDS_P020DESC253 (IDS_P020DESC001 + 252)  /* Description Resouce ID   */
#define IDS_P020DESC254 (IDS_P020DESC001 + 253)  /* Description Resouce ID   */
#define IDS_P020DESC255 (IDS_P020DESC001 + 254)  /* Description Resouce ID   */
#define IDS_P020DESC256 (IDS_P020DESC001 + 255)  /* Description Resouce ID   */
#define IDS_P020DESC257 (IDS_P020DESC001 + 256)  /* Description Resouce ID   */
#define IDS_P020DESC258 (IDS_P020DESC001 + 257)  /* Description Resouce ID   */
#define IDS_P020DESC259 (IDS_P020DESC001 + 258)  /* Description Resouce ID   */
#define IDS_P020DESC260 (IDS_P020DESC001 + 259)  /* Description Resouce ID   */
#define IDS_P020DESC261 (IDS_P020DESC001 + 260)  /* Description Resouce ID   */
#define IDS_P020DESC262 (IDS_P020DESC001 + 261)  /* Description Resouce ID   */
#define IDS_P020DESC263 (IDS_P020DESC001 + 262)  /* Description Resouce ID   */
#define IDS_P020DESC264 (IDS_P020DESC001 + 263)  /* Description Resouce ID   */
#define IDS_P020DESC265 (IDS_P020DESC001 + 264)  /* Description Resouce ID   */
#define IDS_P020DESC266 (IDS_P020DESC001 + 265)  /* Description Resouce ID   */
#define IDS_P020DESC267 (IDS_P020DESC001 + 266)  /* Description Resouce ID   */
#define IDS_P020DESC268 (IDS_P020DESC001 + 267)  /* Description Resouce ID   */
#define IDS_P020DESC269 (IDS_P020DESC001 + 268)  /* Description Resouce ID   */
#define IDS_P020DESC270 (IDS_P020DESC001 + 269)  /* Description Resouce ID   */
#define IDS_P020DESC271 (IDS_P020DESC001 + 270)  /* Description Resouce ID   */
#define IDS_P020DESC272 (IDS_P020DESC001 + 271)  /* Description Resouce ID   */
#define IDS_P020DESC273 (IDS_P020DESC001 + 272)  /* Description Resouce ID   */
#define IDS_P020DESC274 (IDS_P020DESC001 + 273)  /* Description Resouce ID   */
#define IDS_P020DESC275 (IDS_P020DESC001 + 274)  /* Description Resouce ID   */
#define IDS_P020DESC276 (IDS_P020DESC001 + 275)  /* Description Resouce ID   */
#define IDS_P020DESC277 (IDS_P020DESC001 + 276)  /* Description Resouce ID   */
#define IDS_P020DESC278 (IDS_P020DESC001 + 277)  /* Description Resouce ID   */
#define IDS_P020DESC279 (IDS_P020DESC001 + 278)  /* Description Resouce ID   */
#define IDS_P020DESC280 (IDS_P020DESC001 + 279)  /* Description Resouce ID   */
#define IDS_P020DESC281 (IDS_P020DESC001 + 280)  /* Description Resouce ID   */
#define IDS_P020DESC282 (IDS_P020DESC001 + 281)  /* Description Resouce ID   */
#define IDS_P020DESC283 (IDS_P020DESC001 + 282)  /* Description Resouce ID   */
#define IDS_P020DESC284 (IDS_P020DESC001 + 283)  /* Description Resouce ID   */
#define IDS_P020DESC285 (IDS_P020DESC001 + 284)  /* Description Resouce ID   */
#define IDS_P020DESC286 (IDS_P020DESC001 + 285)  /* Description Resouce ID   */
#define IDS_P020DESC287 (IDS_P020DESC001 + 286)  /* Description Resouce ID   */
#define IDS_P020DESC288 (IDS_P020DESC001 + 287)  /* Description Resouce ID   */
#define IDS_P020DESC289 (IDS_P020DESC001 + 288)  /* Description Resouce ID   */
#define IDS_P020DESC290 (IDS_P020DESC001 + 289)  /* Description Resouce ID   */
#define IDS_P020DESC291 (IDS_P020DESC001 + 290)  /* Description Resouce ID   */
#define IDS_P020DESC292 (IDS_P020DESC001 + 291)  /* Description Resouce ID   */
#define IDS_P020DESC293 (IDS_P020DESC001 + 292)  /* Description Resouce ID   */
#define IDS_P020DESC294 (IDS_P020DESC001 + 293)  /* Description Resouce ID   */
#define IDS_P020DESC295 (IDS_P020DESC001 + 294)  /* Description Resouce ID   */
#define IDS_P020DESC296 (IDS_P020DESC001 + 295)  /* Description Resouce ID   */
#define IDS_P020DESC297 (IDS_P020DESC001 + 296)  /* Description Resouce ID   */
#define IDS_P020DESC298 (IDS_P020DESC001 + 297)  /* Description Resouce ID   */
#define IDS_P020DESC299 (IDS_P020DESC001 + 298)  /* Description Resouce ID   */
#define IDS_P020DESC300 (IDS_P020DESC001 + 299)  /* Description Resouce ID   */
#define IDS_P020DESC301 (IDS_P020DESC001 +300)  /* Description Resouce ID   */
#define IDS_P020DESC302 (IDS_P020DESC001 +301)  /* Description Resouce ID   */
#define IDS_P020DESC303 (IDS_P020DESC001 +302)  /* Description Resouce ID   */
#define IDS_P020DESC304 (IDS_P020DESC001 +303)  /* Description Resouce ID   */
#define IDS_P020DESC305 (IDS_P020DESC001 +304)  /* Description Resouce ID   */
#define IDS_P020DESC306 (IDS_P020DESC001 +305)  /* Description Resouce ID   */
#define IDS_P020DESC307 (IDS_P020DESC001 +306)  /* Description Resouce ID   */
#define IDS_P020DESC308 (IDS_P020DESC001 +307)  /* Description Resouce ID   */
#define IDS_P020DESC309 (IDS_P020DESC001 +308)  /* Description Resouce ID   */
#define IDS_P020DESC310 (IDS_P020DESC001 +309)  /* Description Resouce ID   */
#define IDS_P020DESC311 (IDS_P020DESC001 +310)  /* Description Resouce ID   */
#define IDS_P020DESC312 (IDS_P020DESC001 +311)  /* Description Resouce ID   */
#define IDS_P020DESC313 (IDS_P020DESC001 +312)  /* Description Resouce ID   */
#define IDS_P020DESC314 (IDS_P020DESC001 +313)  /* Description Resouce ID   */
#define IDS_P020DESC315 (IDS_P020DESC001 +314)  /* Description Resouce ID   */
#define IDS_P020DESC316 (IDS_P020DESC001 +315)  /* Description Resouce ID   */
#define IDS_P020DESC317 (IDS_P020DESC001 +316)  /* Description Resouce ID   */
#define IDS_P020DESC318 (IDS_P020DESC001 +317)  /* Description Resouce ID   */
#define IDS_P020DESC319 (IDS_P020DESC001 +318)  /* Description Resouce ID   */
#define IDS_P020DESC320 (IDS_P020DESC001 +319)  /* Description Resouce ID   */
#define IDS_P020DESC321 (IDS_P020DESC001 +320)  /* Description Resouce ID   */
#define IDS_P020DESC322 (IDS_P020DESC001 +321)  /* Description Resouce ID   */
#define IDS_P020DESC323 (IDS_P020DESC001 +322)  /* Description Resouce ID   */
#define IDS_P020DESC324 (IDS_P020DESC001 +323)  /* Description Resouce ID   */
#define IDS_P020DESC325 (IDS_P020DESC001 +324)  /* Description Resouce ID   */
#define IDS_P020DESC326 (IDS_P020DESC001 +325)  /* Description Resouce ID   */
#define IDS_P020DESC327 (IDS_P020DESC001 +326)  /* Description Resouce ID   */
#define IDS_P020DESC328 (IDS_P020DESC001 +327)  /* Description Resouce ID   */
#define IDS_P020DESC329 (IDS_P020DESC001 +328)  /* Description Resouce ID   */
#define IDS_P020DESC330 (IDS_P020DESC001 +329)  /* Description Resouce ID   */
#define IDS_P020DESC331 (IDS_P020DESC001 +330)  /* Description Resouce ID   */
#define IDS_P020DESC332 (IDS_P020DESC001 +331)  /* Description Resouce ID   */
#define IDS_P020DESC333 (IDS_P020DESC001 +332)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC334 (IDS_P020DESC001 +333)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC335 (IDS_P020DESC001 +334)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC336 (IDS_P020DESC001 +335)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC337 (IDS_P020DESC001 +336)  /* Description Resouce ID , V3.3 */
#define IDS_P020DESC338 (IDS_P020DESC001 +337)  /* Description Resouce ID , V3.3 for TRN_PRETURN3_ADR */
#define IDS_P020DESC339 (IDS_P020DESC001 +338)  /* Description Resouce ID , V3.3 for TRN_FT_EQUIVALENT */
#define IDS_P020DESC340 (IDS_P020DESC001 +339)  /* Description Resouce ID , V3.3, TRN_GCNO_RETURNS */
#define IDS_P020DESC341 (IDS_P020DESC001 +340)  /* Description Resouce ID , TRN_DSI_CARDTYPE */
#define IDS_P020DESC342 (IDS_P020DESC001 +341)  /* Description Resouce ID , TRN_DSI_CARDNUM  */
#define IDS_P020DESC343 (IDS_P020DESC001 +342)  /* Description Resouce ID , TRN_DSI_CARDEXP  */
#define IDS_P020DESC344 (IDS_P020DESC001 +343)  /* Description Resouce ID , TRN_DSI_AUTHCODE  */
#define IDS_P020DESC345 (IDS_P020DESC001 +344)  /* Description Resouce ID , TRN_DSI_REFNUM  */
#define IDS_P020DESC346 (IDS_P020DESC001 +345)  /* Description Resouce ID , TRN_DSI_AUTHORIZE  */
#define IDS_P020DESC347 (IDS_P020DESC001 +346)  /* Description Resouce ID , TRN_DSI_BALANCE  */
#define IDS_P020DESC348 (IDS_P020DESC001 +347)  /* Description Resouce ID , TRN_DSI_METHOD */
#define IDS_P020DESC349 (IDS_P020DESC001 +348)  /* Description Resouce ID , TRN_DSI_TERMINAL  */
#define IDS_P020DESC350 (IDS_P020DESC001 +349)  /* Description Resouce ID , TRN_DSI_TRANSTYPE  */
#define IDS_P020DESC351 (IDS_P020DESC001 +350)  /* Description Resouce ID , TRN_DSI_TRANSSALE */
#define IDS_P020DESC352 (IDS_P020DESC001 +351)  /* Description Resouce ID , TRN_EJ_PODREPRINT   */
#define IDS_P020DESC353 (IDS_P020DESC001 +352)  /* Description Resouce ID , TRN_TNDR_PRT_DUP_COPY  */
#define IDS_P020DESC354 (IDS_P020DESC001 +353)  /* Description Resouce ID   */
#define IDS_P020DESC355 (IDS_P020DESC001 +354)  /* Description Resouce ID   */
#define IDS_P020DESC356 (IDS_P020DESC001 +355)  /* Description Resouce ID   */
#define IDS_P020DESC357 (IDS_P020DESC001 +356)  /* Description Resouce ID   */
#define IDS_P020DESC358 (IDS_P020DESC001 +357)  /* Description Resouce ID   */
#define IDS_P020DESC359 (IDS_P020DESC001 +358)  /* Description Resouce ID   */
#define IDS_P020DESC360 (IDS_P020DESC001 +359)  /* Description Resouce ID   */
#define IDS_P020DESC361 (IDS_P020DESC001 +360)  /* Description Resouce ID   */
#define IDS_P020DESC362 (IDS_P020DESC001 +361)  /* Description Resouce ID   */
#define IDS_P020DESC363 (IDS_P020DESC001 +362)  /* Description Resouce ID   */
#define IDS_P020DESC364 (IDS_P020DESC001 +363)  /* Description Resouce ID   */
#define IDS_P020DESC365 (IDS_P020DESC001 +364)  /* Description Resouce ID   */
#define IDS_P020DESC366 (IDS_P020DESC001 +365)  /* Description Resouce ID   */
#define IDS_P020DESC367 (IDS_P020DESC001 +366)  /* Description Resouce ID   */
#define IDS_P020DESC368 (IDS_P020DESC001 +367)  /* Description Resouce ID   */
#define IDS_P020DESC369 (IDS_P020DESC001 +368)  /* Description Resouce ID   */
#define IDS_P020DESC370 (IDS_P020DESC001 +369)  /* Description Resouce ID   */
#define IDS_P020DESC371 (IDS_P020DESC001 +370)  /* Description Resouce ID   */
#define IDS_P020DESC372 (IDS_P020DESC001 +371)  /* Description Resouce ID   */
#define IDS_P020DESC373 (IDS_P020DESC001 +372)  /* Description Resouce ID   */
#define IDS_P020DESC374 (IDS_P020DESC001 +373)  /* Description Resouce ID   */
#define IDS_P020DESC375 (IDS_P020DESC001 +374)  /* Description Resouce ID   */
#define IDS_P020DESC376 (IDS_P020DESC001 +375)  /* Description Resouce ID   */
#define IDS_P020DESC377 (IDS_P020DESC001 +376)  /* Description Resouce ID   */
#define IDS_P020DESC378 (IDS_P020DESC001 +377)  /* Description Resouce ID   */
#define IDS_P020DESC379 (IDS_P020DESC001 +378)  /* Description Resouce ID   */
#define IDS_P020DESC380 (IDS_P020DESC001 +379)  /* Description Resouce ID   */
#define IDS_P020DESC381 (IDS_P020DESC001 +380)  /* Description Resouce ID   */
#define IDS_P020DESC382 (IDS_P020DESC001 +381)  /* Description Resouce ID   */
#define IDS_P020DESC383 (IDS_P020DESC001 +382)  /* Description Resouce ID   */
#define IDS_P020DESC384 (IDS_P020DESC001 +383)  /* Description Resouce ID   */
#define IDS_P020DESC385 (IDS_P020DESC001 +384)  /* Description Resouce ID   */
#define IDS_P020DESC386 (IDS_P020DESC001 +385)  /* Description Resouce ID   */
#define IDS_P020DESC387 (IDS_P020DESC001 +386)  /* Description Resouce ID   */
#define IDS_P020DESC388 (IDS_P020DESC001 +387)  /* Description Resouce ID   */
#define IDS_P020DESC389 (IDS_P020DESC001 +388)  /* Description Resouce ID   */
#define IDS_P020DESC390 (IDS_P020DESC001 +389)  /* Description Resouce ID   */
#define IDS_P020DESC391 (IDS_P020DESC001 +390)  /* Description Resouce ID   */
#define IDS_P020DESC392 (IDS_P020DESC001 +391)  /* Description Resouce ID   */
#define IDS_P020DESC393 (IDS_P020DESC001 +392)  /* Description Resouce ID   */
#define IDS_P020DESC394 (IDS_P020DESC001 +393)  /* Description Resouce ID   */
#define IDS_P020DESC395 (IDS_P020DESC001 +394)  /* Description Resouce ID   */
#define IDS_P020DESC396 (IDS_P020DESC001 +395)  /* Description Resouce ID   */
#define IDS_P020DESC397 (IDS_P020DESC001 +396)  /* Description Resouce ID   */
#define IDS_P020DESC398 (IDS_P020DESC001 +397)  /* Description Resouce ID   */
#define IDS_P020DESC399 (IDS_P020DESC001 +398)  /* Description Resouce ID   */
#define IDS_P020DESC400 (IDS_P020DESC001 +399)  /* Description Resouce ID   */





/*** End   NCR2172 ***/
/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P20_ADDR_MAX    MAX_TRANSM_NO       /* Number of Address with       */
                                            /*              Reserved Area   */
#define P20_ADDR_NO     TRN_ADR_MAX         /* Number of Address without    */

/*** NCR2172 ***/                           /*              Reserved Area   */
/*#define P20_TOTAL_MAX   142 */                /* View Number of Address       */
#define P20_TOTAL_MAX   (MAX_TRANSM_NO - 1)     /* View Number of Address, table awSelectData does not contain all mnemonic addresses  */
/* NOTE: This MUST change to 399, once the database changes are being implemented,
currently PEP is only reading the first 200, but there is room for 400  JHHJ 3-25-04*/
                                            /*                  Total  V3.3 */

//#define P20_MNE_LEN     8                   /* Max Length of Mnemonic       */
#define P20_MNE_LEN     20                   /* Max Length of Mnemonic  Database Changes 8->20 JHHJ*/

#define P20_DESC_LEN    64                  /* Max Length of Description    */

#define P20_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P20_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P20_PAGE_MAX    20                  /* No. of Items in One Page     */
#define P20_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P20_ONE_PAGE    (P20_PAGE_MAX - P20_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P20_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */
/* #define P20_SCBAR_MAX   (TRN_ADR_MAX - P20_PAGE_MAX) */
#define P20_SCBAR_MAX   (P20_TOTAL_MAX - P20_PAGE_MAX)  /* V3.3 */


/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P20GETINDEX(id, top)   (((id) - IDD_P20_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P20_MNE_LEN];       /* Mnemonic Data */
} P20MNEMO, FAR *LPP20MNEMO;

//NOTE CHANGES took +1 off that ^

typedef struct  _Work {
            WCHAR    szWork[P20_MNE_LEN + 1];   /* Mnemonic Work Area */
} P20WORK, FAR *LPP20WORK;

/*
* ===========================================================================
*       External Work Area Declarations
* ===========================================================================
*/
/*
* ===========================================================================
*       Function Prototype Declarations
* ===========================================================================
*/
VOID    P20InitDlg(HWND);
VOID    P20GetMnemo(HWND, WORD wItem);
VOID    P20DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P20ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P20CalcIndex(WORD wScrlCode);
VOID    P20RedrawText(HWND);

/* ===== End of File (P020.H) ===== */
