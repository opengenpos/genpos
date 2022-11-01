/*
* ---------------------------------------------------------------------------
* Title         :   Lead-Through Messages Header File (Prog. 21)
* Category      :   Setup, AT&T 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P021.H
* Copyright (C) :   1995, AT&T Corp. E&M-OISO, All rights reserved.
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
* Apr-28-94 : 00.00.02 : M.Yamamoto : Add Address No. 85
* Apr-17-95 : 03.00.00 : H.Ishida   : Add Address NO. 86-92
* Jul-25-95 : 03.00.00 : hkato      : R3.0 FVT Comment.
* Jan-24-96 : 03.00.05 : M.Suzuki   : R3.1
* Sep-08-98 : 03.03.00 : A.Mitsui   : V3.3
* Nov-19-99 :          : K.Yanagida : NCR2172
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
#define IDD_P21_SCBAR   22101

#define IDD_P21_DESC01  (IDD_P21_SCBAR +  1)
#define IDD_P21_DESC02  (IDD_P21_SCBAR +  2)
#define IDD_P21_DESC03  (IDD_P21_SCBAR +  3)
#define IDD_P21_DESC04  (IDD_P21_SCBAR +  4)
#define IDD_P21_DESC05  (IDD_P21_SCBAR +  5)
#define IDD_P21_DESC06  (IDD_P21_SCBAR +  6)
#define IDD_P21_DESC07  (IDD_P21_SCBAR +  7)
#define IDD_P21_DESC08  (IDD_P21_SCBAR +  8)
#define IDD_P21_DESC09  (IDD_P21_SCBAR +  9)
#define IDD_P21_DESC10  (IDD_P21_SCBAR + 10)

#define IDD_P21_EDIT01  (IDD_P21_SCBAR + 11)
#define IDD_P21_EDIT02  (IDD_P21_SCBAR + 12)
#define IDD_P21_EDIT03  (IDD_P21_SCBAR + 13)
#define IDD_P21_EDIT04  (IDD_P21_SCBAR + 14)
#define IDD_P21_EDIT05  (IDD_P21_SCBAR + 15)
#define IDD_P21_EDIT06  (IDD_P21_SCBAR + 16)
#define IDD_P21_EDIT07  (IDD_P21_SCBAR + 17)
#define IDD_P21_EDIT08  (IDD_P21_SCBAR + 18)
#define IDD_P21_EDIT09  (IDD_P21_SCBAR + 19)
#define IDD_P21_EDIT10  (IDD_P21_SCBAR + 20)

#define IDD_P21_ADDR	22130
#define IDD_P21_DESC	22131
#define IDD_P21_MNEM	22132
#define IDD_P21_CAPTION	22133

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P021DESC001 IDS_P21                 /* Description Resouce ID   */
#define IDS_P021DESC002 (IDS_P021DESC001 +  1)  /* Description Resouce ID   */
#define IDS_P021DESC003 (IDS_P021DESC001 +  2)  /* Description Resouce ID   */
#define IDS_P021DESC004 (IDS_P021DESC001 +  3)  /* Description Resouce ID   */
#define IDS_P021DESC005 (IDS_P021DESC001 +  4)  /* Description Resouce ID   */
#define IDS_P021DESC006 (IDS_P021DESC001 +  5)  /* Description Resouce ID   */
#define IDS_P021DESC007 (IDS_P021DESC001 +  6)  /* Description Resouce ID   */
#define IDS_P021DESC008 (IDS_P021DESC001 +  7)  /* Description Resouce ID   */
#define IDS_P021DESC009 (IDS_P021DESC001 +  8)  /* Description Resouce ID   */
#define IDS_P021DESC010 (IDS_P021DESC001 +  9)  /* Description Resouce ID   */
#define IDS_P021DESC011 (IDS_P021DESC001 + 10)  /* Description Resouce ID   */
#define IDS_P021DESC012 (IDS_P021DESC001 + 11)  /* Description Resouce ID   */
#define IDS_P021DESC013 (IDS_P021DESC001 + 12)  /* Description Resouce ID   */
#define IDS_P021DESC014 (IDS_P021DESC001 + 13)  /* Description Resouce ID   */
#define IDS_P021DESC015 (IDS_P021DESC001 + 14)  /* Description Resouce ID   */
#define IDS_P021DESC016 (IDS_P021DESC001 + 15)  /* Description Resouce ID   */
#define IDS_P021DESC017 (IDS_P021DESC001 + 16)  /* Description Resouce ID   */
#define IDS_P021DESC018 (IDS_P021DESC001 + 17)  /* Description Resouce ID   */
#define IDS_P021DESC019 (IDS_P021DESC001 + 18)  /* Description Resouce ID   */
#define IDS_P021DESC020 (IDS_P021DESC001 + 19)  /* Description Resouce ID   */
#define IDS_P021DESC021 (IDS_P021DESC001 + 20)  /* Description Resouce ID   */
#define IDS_P021DESC022 (IDS_P021DESC001 + 21)  /* Description Resouce ID   */
#define IDS_P021DESC023 (IDS_P021DESC001 + 22)  /* Description Resouce ID   */
#define IDS_P021DESC024 (IDS_P021DESC001 + 23)  /* Description Resouce ID   */
#define IDS_P021DESC025 (IDS_P021DESC001 + 24)  /* Description Resouce ID   */
#define IDS_P021DESC026 (IDS_P021DESC001 + 25)  /* Description Resouce ID   */
#define IDS_P021DESC027 (IDS_P021DESC001 + 26)  /* Description Resouce ID   */
#define IDS_P021DESC028 (IDS_P021DESC001 + 27)  /* Description Resouce ID   */
#define IDS_P021DESC029 (IDS_P021DESC001 + 28)  /* Description Resouce ID   */
#define IDS_P021DESC030 (IDS_P021DESC001 + 29)  /* Description Resouce ID   */
#define IDS_P021DESC031 (IDS_P021DESC001 + 30)  /* Description Resouce ID   */
#define IDS_P021DESC032 (IDS_P021DESC001 + 31)  /* Description Resouce ID   */
#define IDS_P021DESC033 (IDS_P021DESC001 + 32)  /* Description Resouce ID   */
#define IDS_P021DESC034 (IDS_P021DESC001 + 33)  /* Description Resouce ID   */
#define IDS_P021DESC035 (IDS_P021DESC001 + 34)  /* Description Resouce ID   */
#define IDS_P021DESC036 (IDS_P021DESC001 + 35)  /* Description Resouce ID   */
#define IDS_P021DESC037 (IDS_P021DESC001 + 36)  /* Description Resouce ID   */
#define IDS_P021DESC038 (IDS_P021DESC001 + 37)  /* Description Resouce ID   */
#define IDS_P021DESC039 (IDS_P021DESC001 + 38)  /* Description Resouce ID   */
#define IDS_P021DESC040 (IDS_P021DESC001 + 39)  /* Description Resouce ID   */
#define IDS_P021DESC041 (IDS_P021DESC001 + 40)  /* Description Resouce ID   */
#define IDS_P021DESC042 (IDS_P021DESC001 + 41)  /* Description Resouce ID   */
#define IDS_P021DESC043 (IDS_P021DESC001 + 42)  /* Description Resouce ID   */
#define IDS_P021DESC044 (IDS_P021DESC001 + 43)  /* Description Resouce ID   */
#define IDS_P021DESC045 (IDS_P021DESC001 + 44)  /* Description Resouce ID   */
#define IDS_P021DESC046 (IDS_P021DESC001 + 45)  /* Description Resouce ID   */
#define IDS_P021DESC047 (IDS_P021DESC001 + 46)  /* Description Resouce ID   */
#define IDS_P021DESC048 (IDS_P021DESC001 + 47)  /* Description Resouce ID   */
#define IDS_P021DESC049 (IDS_P021DESC001 + 48)  /* Description Resouce ID   */
#define IDS_P021DESC050 (IDS_P021DESC001 + 49)  /* Description Resouce ID   */
#define IDS_P021DESC051 (IDS_P021DESC001 + 50)  /* Description Resouce ID   */
#define IDS_P021DESC052 (IDS_P021DESC001 + 51)  /* Description Resouce ID   */
#define IDS_P021DESC053 (IDS_P021DESC001 + 52)  /* Description Resouce ID   */
#define IDS_P021DESC054 (IDS_P021DESC001 + 53)  /* Description Resouce ID   */
#define IDS_P021DESC055 (IDS_P021DESC001 + 54)  /* Description Resouce ID   */
#define IDS_P021DESC056 (IDS_P021DESC001 + 55)  /* Description Resouce ID   */
#define IDS_P021DESC057 (IDS_P021DESC001 + 56)  /* Description Resouce ID   */
#define IDS_P021DESC058 (IDS_P021DESC001 + 57)  /* Description Resouce ID   */
#define IDS_P021DESC059 (IDS_P021DESC001 + 58)  /* Description Resouce ID   */
#define IDS_P021DESC060 (IDS_P021DESC001 + 59)  /* Description Resouce ID   */
#define IDS_P021DESC061 (IDS_P021DESC001 + 60)  /* Description Resouce ID   */
#define IDS_P021DESC062 (IDS_P021DESC001 + 61)  /* Description Resouce ID   */
#define IDS_P021DESC063 (IDS_P021DESC001 + 62)  /* Description Resouce ID   */
#define IDS_P021DESC064 (IDS_P021DESC001 + 63)  /* Description Resouce ID   */

#define IDS_P021DESC065 (IDS_P021DESC001 + 64)  /* Description Resouce ID   */
#define IDS_P021DESC066 (IDS_P021DESC001 + 65)  /* Description Resouce ID   */
#define IDS_P021DESC067 (IDS_P021DESC001 + 66)  /* Description Resouce ID   */
#define IDS_P021DESC068 (IDS_P021DESC001 + 67)  /* Description Resouce ID   */
#define IDS_P021DESC069 (IDS_P021DESC001 + 68)  /* Description Resouce ID   */
#define IDS_P021DESC070 (IDS_P021DESC001 + 69)  /* Description Resouce ID   */
#define IDS_P021DESC071 (IDS_P021DESC001 + 70)  /* Description Resouce ID   */
#define IDS_P021DESC072 (IDS_P021DESC001 + 71)  /* Description Resouce ID   */
#define IDS_P021DESC073 (IDS_P021DESC001 + 72)  /* Description Resouce ID   */
#define IDS_P021DESC074 (IDS_P021DESC001 + 73)  /* Description Resouce ID   */
#define IDS_P021DESC075 (IDS_P021DESC001 + 74)  /* Description Resouce ID   */
#define IDS_P021DESC076 (IDS_P021DESC001 + 75)  /* Description Resouce ID   */
#define IDS_P021DESC077 (IDS_P021DESC001 + 76)  /* Description Resouce ID   */
#define IDS_P021DESC078 (IDS_P021DESC001 + 77)  /* Description Resouce ID   */
#define IDS_P021DESC079 (IDS_P021DESC001 + 78)  /* Description Resouce ID   */
#define IDS_P021DESC080 (IDS_P021DESC001 + 79)  /* Description Resouce ID   */
#define IDS_P021DESC081 (IDS_P021DESC001 + 80)  /* Description Resouce ID   */
#define IDS_P021DESC082 (IDS_P021DESC001 + 81)  /* Description Resouce ID   */
#define IDS_P021DESC083 (IDS_P021DESC001 + 82)  /* Description Resouce ID   */
#define IDS_P021DESC084 (IDS_P021DESC001 + 83)  /* Description Resouce ID   */
#define IDS_P021DESC085 (IDS_P021DESC001 + 84)  /* Description Resouce ID   */
#define IDS_P021DESC086 (IDS_P021DESC001 + 85)  /* Description Resouce ID   */
#define IDS_P021DESC087 (IDS_P021DESC001 + 86)  /* Description Resouce ID   */
#define IDS_P021DESC088 (IDS_P021DESC001 + 87)  /* Description Resouce ID   */
#define IDS_P021DESC089 (IDS_P021DESC001 + 88)  /* Description Resouce ID   */
#define IDS_P021DESC090 (IDS_P021DESC001 + 89)  /* Description Resouce ID   */
#define IDS_P021DESC091 (IDS_P021DESC001 + 90)  /* Description Resouce ID   */
#define IDS_P021DESC092 (IDS_P021DESC001 + 91)  /* Description Resouce ID   */
#define IDS_P021DESC093 (IDS_P021DESC001 + 92)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC094 (IDS_P021DESC001 + 93)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC095 (IDS_P021DESC001 + 94)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC096 (IDS_P021DESC001 + 95)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC097 (IDS_P021DESC001 + 96)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC098 (IDS_P021DESC001 + 97)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC099 (IDS_P021DESC001 + 98)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC100 (IDS_P021DESC001 + 99)  /* Description Resouce ID, R3.0 */
#define IDS_P021DESC101 (IDS_P021DESC001 +100)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC102 (IDS_P021DESC001 +101)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC103 (IDS_P021DESC001 +102)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC104 (IDS_P021DESC001 +103)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC105 (IDS_P021DESC001 +104)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC106 (IDS_P021DESC001 +105)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC107 (IDS_P021DESC001 +106)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC108 (IDS_P021DESC001 +107)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC109 (IDS_P021DESC001 +108)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC110 (IDS_P021DESC001 +109)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC111 (IDS_P021DESC001 +110)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC112 (IDS_P021DESC001 +111)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC113 (IDS_P021DESC001 +112)  /* Description Resouce ID, R3.1 */
#define IDS_P021DESC114 (IDS_P021DESC001 +113)  /* Description Resouce ID, R3.1 */

#define IDS_P021DESC115 (IDS_P021DESC001 +114)  /* Description Resouce ID, V3.3 */
#define IDS_P021DESC116 (IDS_P021DESC001 +115)  /* Description Resouce ID, V3.3 */
#define IDS_P021DESC117 (IDS_P021DESC001 +116)  /* Description Resouce ID, V3.3 */
#define IDS_P021DESC118 (IDS_P021DESC001 +117)  /* Description Resouce ID, V3.3 */
#define IDS_P021DESC119 (IDS_P021DESC001 +118)  /* Description Resouce ID, V3.3 */
#define IDS_P021DESC120 (IDS_P021DESC001 +119)  /* Description Resouce ID, V3.3 */

/*** Start NCR2172 ***/
#define IDS_P021DESC121 (IDS_P021DESC001 +120)  /* Description Resouce ID */
#define IDS_P021DESC122 (IDS_P021DESC001 +121)  /* Description Resouce ID */
#define IDS_P021DESC123 (IDS_P021DESC001 +122)  /* Description Resouce ID */
#define IDS_P021DESC124 (IDS_P021DESC001 +123)  /* Description Resouce ID */
#define IDS_P021DESC125 (IDS_P021DESC001 +124)  /* Description Resouce ID */
#define IDS_P021DESC126 (IDS_P021DESC001 +125)  /* Description Resouce ID */
#define IDS_P021DESC127 (IDS_P021DESC001 +126)  /* Description Resouce ID */
#define IDS_P021DESC128 (IDS_P021DESC001 +127)  /* Description Resouce ID */
#define IDS_P021DESC129 (IDS_P021DESC001 +128)  /* Description Resouce ID */
#define IDS_P021DESC130 (IDS_P021DESC001 +129)  /* Description Resouce ID */
#define IDS_P021DESC131 (IDS_P021DESC001 +130)  /* Description Resouce ID */
#define IDS_P021DESC132 (IDS_P021DESC001 +131)  /* Description Resouce ID */
#define IDS_P021DESC133 (IDS_P021DESC001 +132)  /* Description Resouce ID */
#define IDS_P021DESC134 (IDS_P021DESC001 +133)  /* Description Resouce ID */
#define IDS_P021DESC135 (IDS_P021DESC001 +134)  /* Description Resouce ID */
#define IDS_P021DESC136 (IDS_P021DESC001 +135)  /* Description Resouce ID */
#define IDS_P021DESC137 (IDS_P021DESC001 +136)  /* Description Resouce ID */
#define IDS_P021DESC138 (IDS_P021DESC001 +137)  /* Description Resouce ID */
#define IDS_P021DESC139 (IDS_P021DESC001 +138)  /* Description Resouce ID */
#define IDS_P021DESC140 (IDS_P021DESC001 +139)  /* Description Resouce ID */
#define IDS_P021DESC141 (IDS_P021DESC001 +140)  /* Description Resouce ID */
#define IDS_P021DESC142 (IDS_P021DESC001 +141)  /* Description Resouce ID */
#define IDS_P021DESC143 (IDS_P021DESC001 +142)  /* Description Resouce ID */
#define IDS_P021DESC144 (IDS_P021DESC001 +143)  /* Description Resouce ID */
#define IDS_P021DESC145 (IDS_P021DESC001 +144)  /* Description Resouce ID */
#define IDS_P021DESC146 (IDS_P021DESC001 +145)  /* Description Resouce ID */
#define IDS_P021DESC147 (IDS_P021DESC001 +146)  /* Description Resouce ID */
#define IDS_P021DESC148 (IDS_P021DESC001 +147)  /* Description Resouce ID */
#define IDS_P021DESC149 (IDS_P021DESC001 +148)  /* Description Resouce ID */
#define IDS_P021DESC150 (IDS_P021DESC001 +149)  /* Description Resouce ID */
#define IDS_P021DESC151 (IDS_P021DESC001 +150)  /* Description Resouce ID */
#define IDS_P021DESC152 (IDS_P021DESC001 +151)  /* Description Resouce ID */
#define IDS_P021DESC153 (IDS_P021DESC001 +152)  /* Description Resouce ID */
#define IDS_P021DESC154 (IDS_P021DESC001 +153)  /* Description Resouce ID */
#define IDS_P021DESC155 (IDS_P021DESC001 +154)  /* Description Resouce ID */
#define IDS_P021DESC156 (IDS_P021DESC001 +155)  /* Description Resouce ID */
#define IDS_P021DESC157 (IDS_P021DESC001 +156)  /* Description Resouce ID */
#define IDS_P021DESC158 (IDS_P021DESC001 +157)  /* Description Resouce ID */
#define IDS_P021DESC159 (IDS_P021DESC001 +158)  /* Description Resouce ID */
#define IDS_P021DESC160 (IDS_P021DESC001 +159)  /* Description Resouce ID */
#define IDS_P021DESC161 (IDS_P021DESC001 +160)  /* Description Resouce ID */
#define IDS_P021DESC162 (IDS_P021DESC001 +161)  /* Description Resouce ID */
#define IDS_P021DESC163 (IDS_P021DESC001 +162)  /* Description Resouce ID */
#define IDS_P021DESC164 (IDS_P021DESC001 +163)  /* Description Resouce ID */
#define IDS_P021DESC165 (IDS_P021DESC001 +164)  /* Description Resouce ID */
#define IDS_P021DESC166 (IDS_P021DESC001 +165)  /* Description Resouce ID */
#define IDS_P021DESC167 (IDS_P021DESC001 +166)  /* Description Resouce ID */
#define IDS_P021DESC168 (IDS_P021DESC001 +167)  /* Description Resouce ID */
#define IDS_P021DESC169 (IDS_P021DESC001 +168)  /* Description Resouce ID */
#define IDS_P021DESC170 (IDS_P021DESC001 +169)  /* Description Resouce ID */
#define IDS_P021DESC171 (IDS_P021DESC001 +170)  /* Description Resouce ID */
#define IDS_P021DESC172 (IDS_P021DESC001 +171)  /* Description Resouce ID */
#define IDS_P021DESC173 (IDS_P021DESC001 +172)  /* Description Resouce ID */
#define IDS_P021DESC174 (IDS_P021DESC001 +173)  /* Description Resouce ID */
#define IDS_P021DESC175 (IDS_P021DESC001 +174)  /* Description Resouce ID */
#define IDS_P021DESC176 (IDS_P021DESC001 +175)  /* Description Resouce ID */
#define IDS_P021DESC177 (IDS_P021DESC001 +176)  /* Description Resouce ID */
#define IDS_P021DESC178 (IDS_P021DESC001 +177)  /* Description Resouce ID */
#define IDS_P021DESC179 (IDS_P021DESC001 +178)  /* Description Resouce ID */
#define IDS_P021DESC180 (IDS_P021DESC001 +179)  /* Description Resouce ID */
#define IDS_P021DESC181 (IDS_P021DESC001 +180)  /* Description Resouce ID */
#define IDS_P021DESC182 (IDS_P021DESC001 +181)  /* Description Resouce ID */
#define IDS_P021DESC183 (IDS_P021DESC001 +182)  /* Description Resouce ID */
#define IDS_P021DESC184 (IDS_P021DESC001 +183)  /* Description Resouce ID */
#define IDS_P021DESC185 (IDS_P021DESC001 +184)  /* Description Resouce ID */
#define IDS_P021DESC186 (IDS_P021DESC001 +185)  /* Description Resouce ID */
#define IDS_P021DESC187 (IDS_P021DESC001 +186)  /* Description Resouce ID */
#define IDS_P021DESC188 (IDS_P021DESC001 +187)  /* Description Resouce ID */
#define IDS_P021DESC189 (IDS_P021DESC001 +188)  /* Description Resouce ID */
#define IDS_P021DESC190 (IDS_P021DESC001 +189)  /* Description Resouce ID */
#define IDS_P021DESC191 (IDS_P021DESC001 +190)  /* Description Resouce ID */
#define IDS_P021DESC192 (IDS_P021DESC001 +191)  /* Description Resouce ID */
#define IDS_P021DESC193 (IDS_P021DESC001 +192)  /* Description Resouce ID */
#define IDS_P021DESC194 (IDS_P021DESC001 +193)  /* Description Resouce ID */
#define IDS_P021DESC195 (IDS_P021DESC001 +194)  /* Description Resouce ID */
#define IDS_P021DESC196 (IDS_P021DESC001 +195)  /* Description Resouce ID */
#define IDS_P021DESC197 (IDS_P021DESC001 +196)  /* Description Resouce ID */
#define IDS_P021DESC198 (IDS_P021DESC001 +197)  /* Description Resouce ID */
#define IDS_P021DESC199 (IDS_P021DESC001 +198)  /* Description Resouce ID */
#define IDS_P021DESC200 (IDS_P021DESC001 +199)  /* Description Resouce ID */
#define IDS_P021DESC201 (IDS_P021DESC001 +200)  /* Description Resouce ID */
#define IDS_P021DESC202 (IDS_P021DESC001 +201)  /* Description Resouce ID */
#define IDS_P021DESC203 (IDS_P021DESC001 +202)  /* Description Resouce ID */
#define IDS_P021DESC204 (IDS_P021DESC001 +203)  /* Description Resouce ID */
#define IDS_P021DESC205 (IDS_P021DESC001 +204)  /* Description Resouce ID */
#define IDS_P021DESC206 (IDS_P021DESC001 +205)  /* Description Resouce ID */
#define IDS_P021DESC207 (IDS_P021DESC001 +206)  /* Description Resouce ID */
#define IDS_P021DESC208 (IDS_P021DESC001 +207)  /* Description Resouce ID */
#define IDS_P021DESC209 (IDS_P021DESC001 +208)  /* Description Resouce ID */
#define IDS_P021DESC210 (IDS_P021DESC001 +209)  /* Description Resouce ID */
#define IDS_P021DESC211 (IDS_P021DESC001 +210)  /* Description Resouce ID */
#define IDS_P021DESC212 (IDS_P021DESC001 +211)  /* Description Resouce ID */
#define IDS_P021DESC213 (IDS_P021DESC001 +212)  /* Description Resouce ID */
#define IDS_P021DESC214 (IDS_P021DESC001 +213)  /* Description Resouce ID */
#define IDS_P021DESC215 (IDS_P021DESC001 +214)  /* Description Resouce ID */
#define IDS_P021DESC216 (IDS_P021DESC001 +215)  /* Description Resouce ID */
#define IDS_P021DESC217 (IDS_P021DESC001 +216)  /* Description Resouce ID */
#define IDS_P021DESC218 (IDS_P021DESC001 +217)  /* Description Resouce ID */
#define IDS_P021DESC219 (IDS_P021DESC001 +218)  /* Description Resouce ID */
#define IDS_P021DESC220 (IDS_P021DESC001 +219)  /* Description Resouce ID */
/*** End   NCR2172 ***/

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/

//  WARNING:  When changing P21_TOTAL_MAX you must also modify the values in the
//				table awSelectData[P21_TOTAL_MAX] in file P021.C
#define P21_ADDR_MAX    MAX_LEAD_NO         /* Number of Address with       */
                                            /*              Reserved Area   */
#define P21_ADDR_NO     LDT_ADR_MAX         /* Number of Address without    */

#define P21_TOTAL_MAX   (MAX_LEAD_NO - 1)   /* View Number of Address Total V3.3, table awSelectData does not contain all mnemonic addresses */

#define P21_MNE_LEN     PARA_LEADTHRU_LEN   /* Max Length of Mnemonic in UNINIRAM */
#define P21_DESC_LEN    82                  /* Max Length of Description    */

#define P21_ERR_LEN     PEP_ALLOC_LEN       /* Max Length of Error Message  */
#define P21_CAP_LEN     PEP_CAPTION_LEN     /* Length of MessageBox Caption */

#define P21_PAGE_MAX    10                  /* No. of Items in One Page     */
#define P21_ONE_LINE    1                   /* Scroll to One Line Up/ Down  */
#define P21_ONE_PAGE    (P21_PAGE_MAX - P21_ONE_LINE)
                                            /* Scroll to One Page Up/ Down  */

#define P21_SCBAR_MIN   0                   /* Min. Value of Scroll Range   */

/* #define P21_SCBAR_MAX   (P21_ADDR_NO - P21_PAGE_MAX) */
#define P21_SCBAR_MAX   (P21_TOTAL_MAX - P21_PAGE_MAX)      /* V3.3 */

/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/
#define P21GETINDEX(id, top)   (((id) - IDD_P21_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/
typedef struct  _Mnemonic {
            WCHAR    chMnemo[P21_MNE_LEN];       /* Mnemonic Data */
} P21MNEMO, FAR *LPP21MNEMO;

typedef struct  _Work {
            WCHAR    szWork[P21_MNE_LEN + 1];   /* Mnemonic Work Area */
} P21WORK, FAR *LPP21WORK;

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
VOID    P21InitDlg(HWND);
VOID    P21GetMnemo(HWND, WORD wItem);
VOID    P21DispAllocErr(HWND, HGLOBAL, HGLOBAL);
BOOL    P21ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P21CalcIndex(WORD);
VOID    P21RedrawText(HWND);

/* ===== End of File (P021.H) ===== */
