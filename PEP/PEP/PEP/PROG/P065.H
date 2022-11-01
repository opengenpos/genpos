/*
* ---------------------------------------------------------------------------
* Title         :   Multi-Line Display Mnemonics Header File (Prog. 65)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P065.H
* Copyright (C) :   1998, NCR Corp. E&M-OISO, All rights reserved.
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
* Sep-08-98 : 03.03.00 : A.Mitsui   : Initial
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
#define IDD_P65_SCBAR   26501

#define IDD_P65_DESC01  (IDD_P65_SCBAR +  1)
#define IDD_P65_DESC02  (IDD_P65_SCBAR +  2)
#define IDD_P65_DESC03  (IDD_P65_SCBAR +  3)
#define IDD_P65_DESC04  (IDD_P65_SCBAR +  4)
#define IDD_P65_DESC05  (IDD_P65_SCBAR +  5)
#define IDD_P65_DESC06  (IDD_P65_SCBAR +  6)
#define IDD_P65_DESC07  (IDD_P65_SCBAR +  7)
#define IDD_P65_DESC08  (IDD_P65_SCBAR +  8)
#define IDD_P65_DESC09  (IDD_P65_SCBAR +  9)
#define IDD_P65_DESC10  (IDD_P65_SCBAR + 10)

#define IDD_P65_EDIT01  (IDD_P65_SCBAR + 11)
#define IDD_P65_EDIT02  (IDD_P65_SCBAR + 12)
#define IDD_P65_EDIT03  (IDD_P65_SCBAR + 13)
#define IDD_P65_EDIT04  (IDD_P65_SCBAR + 14)
#define IDD_P65_EDIT05  (IDD_P65_SCBAR + 15)
#define IDD_P65_EDIT06  (IDD_P65_SCBAR + 16)
#define IDD_P65_EDIT07  (IDD_P65_SCBAR + 17)
#define IDD_P65_EDIT08  (IDD_P65_SCBAR + 18)
#define IDD_P65_EDIT09  (IDD_P65_SCBAR + 19)
#define IDD_P65_EDIT10  (IDD_P65_SCBAR + 20)

#define IDD_P65_CAPTION1	26525
#define	IDD_P65_ADDR		26526
#define	IDD_P65_DESC		26527
#define IDD_P65_MNEM		26528

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P065DESC001 IDS_P65                 /* Description Resource ID   */
#define IDS_P065DESC002 (IDS_P065DESC001 +  1)  /* Description Resource ID   */
#define IDS_P065DESC003 (IDS_P065DESC001 +  2)  /* Description Resource ID   */
#define IDS_P065DESC004 (IDS_P065DESC001 +  3)  /* Description Resource ID   */
#define IDS_P065DESC005 (IDS_P065DESC001 +  4)  /* Description Resource ID   */
#define IDS_P065DESC006 (IDS_P065DESC001 +  5)  /* Description Resource ID   */
#define IDS_P065DESC007 (IDS_P065DESC001 +  6)  /* Description Resource ID   */
#define IDS_P065DESC008 (IDS_P065DESC001 +  7)  /* Description Resource ID   */
#define IDS_P065DESC009 (IDS_P065DESC001 +  8)  /* Description Resource ID   */
#define IDS_P065DESC010 (IDS_P065DESC001 +  9)  /* Description Resource ID   */
#define IDS_P065DESC011 (IDS_P065DESC001 + 10)  /* Description Resource ID   */
#define IDS_P065DESC012 (IDS_P065DESC001 + 11)  /* Description Resource ID   */
#define IDS_P065DESC013 (IDS_P065DESC001 + 12)  /* Description Resource ID   */
#define IDS_P065DESC014 (IDS_P065DESC001 + 13)  /* Description Resource ID   */
#define IDS_P065DESC015 (IDS_P065DESC001 + 14)  /* Description Resource ID   */
#define IDS_P065DESC016 (IDS_P065DESC001 + 15)  /* Description Resource ID   */
#define IDS_P065DESC017 (IDS_P065DESC001 + 16)  /* Description Resource ID   */
#define IDS_P065DESC018 (IDS_P065DESC001 + 17)  /* Description Resource ID   */
#define IDS_P065DESC019 (IDS_P065DESC001 + 18)  /* Description Resource ID   */
#define IDS_P065DESC020 (IDS_P065DESC001 + 19)  /* Description Resource ID   */
#define IDS_P065DESC021 (IDS_P065DESC001 + 20)  /* Description Resource ID   */
#define IDS_P065DESC022 (IDS_P065DESC001 + 21)  /* Description Resource ID   */
#define IDS_P065DESC023 (IDS_P065DESC001 + 22)  /* Description Resource ID   */
#define IDS_P065DESC024 (IDS_P065DESC001 + 23)  /* Description Resource ID   */
#define IDS_P065DESC025 (IDS_P065DESC001 + 24)  /* Description Resource ID   */
#define IDS_P065DESC026 (IDS_P065DESC001 + 25)  /* Description Resource ID   */
#define IDS_P065DESC027 (IDS_P065DESC001 + 26)  /* Description Resource ID   */
#define IDS_P065DESC028 (IDS_P065DESC001 + 27)  /* Description Resource ID   */
#define IDS_P065DESC029 (IDS_P065DESC001 + 28)  /* Description Resource ID   */
#define IDS_P065DESC030 (IDS_P065DESC001 + 29)  /* Description Resource ID   */
#define IDS_P065DESC031 (IDS_P065DESC001 + 30)  /* Description Resource ID   */
#define IDS_P065DESC032 (IDS_P065DESC001 + 31)  /* Description Resource ID   */
#define IDS_P065DESC033 (IDS_P065DESC001 + 32)  /* Description Resource ID   */
#define IDS_P065DESC034 (IDS_P065DESC001 + 33)  /* Description Resource ID   */
#define IDS_P065DESC035 (IDS_P065DESC001 + 34)  /* Description Resource ID   */
#define IDS_P065DESC036 (IDS_P065DESC001 + 35)  /* Description Resource ID   */
#define IDS_P065DESC037 (IDS_P065DESC001 + 36)  /* Description Resource ID   */
#define IDS_P065DESC038 (IDS_P065DESC001 + 37)  /* Description Resource ID   */
#define IDS_P065DESC039 (IDS_P065DESC001 + 38)  /* Description Resource ID   */
#define IDS_P065DESC040 (IDS_P065DESC001 + 39)  /* Description Resource ID   */
#define IDS_P065DESC041 (IDS_P065DESC001 + 40)  /* Description Resource ID   */
#define IDS_P065DESC042 (IDS_P065DESC001 + 41)  /* Description Resource ID   */
#define IDS_P065DESC043 (IDS_P065DESC001 + 42)  /* Description Resource ID   */
#define IDS_P065DESC044 (IDS_P065DESC001 + 43)  /* Description Resource ID   */
#define IDS_P065DESC045 (IDS_P065DESC001 + 44)  /* Description Resource ID   */
#define IDS_P065DESC046 (IDS_P065DESC001 + 45)  /* Description Resource ID   */
#define IDS_P065DESC047 (IDS_P065DESC001 + 46)  /* Description Resource ID   */
#define IDS_P065DESC048 (IDS_P065DESC001 + 47)  /* Description Resource ID   */
#define IDS_P065DESC049 (IDS_P065DESC001 + 48)  /* Description Resource ID   */
#define IDS_P065DESC050 (IDS_P065DESC001 + 49)  /* Description Resource ID   */
#define IDS_P065DESC051 (IDS_P065DESC001 + 50)  /* Description Resource ID   */
#define IDS_P065DESC052 (IDS_P065DESC001 + 51)  /* Description Resource ID   */
#define IDS_P065DESC053 (IDS_P065DESC001 + 52)  /* Description Resource ID   */
#define IDS_P065DESC054 (IDS_P065DESC001 + 53)  /* Description Resource ID   */
#define IDS_P065DESC055 (IDS_P065DESC001 + 54)  /* Description Resource ID   */
#define IDS_P065DESC056 (IDS_P065DESC001 + 55)  /* Description Resource ID   */
#define IDS_P065DESC057 (IDS_P065DESC001 + 56)  /* Description Resource ID   */
#define IDS_P065DESC058 (IDS_P065DESC001 + 57)  /* Description Resource ID   */
#define IDS_P065DESC059 (IDS_P065DESC001 + 58)  /* Description Resource ID   */
#define IDS_P065DESC060 (IDS_P065DESC001 + 59)  /* Description Resource ID   */
#define IDS_P065DESC061 (IDS_P065DESC001 + 60)  /* Description Resource ID   */
#define IDS_P065DESC062 (IDS_P065DESC001 + 61)  /* Description Resource ID   */
#define IDS_P065DESC063 (IDS_P065DESC001 + 62)  /* Description Resource ID   */
#define IDS_P065DESC064 (IDS_P065DESC001 + 63)  /* Description Resource ID   */
#define IDS_P065DESC065 (IDS_P065DESC001 + 64)  /* Description Resource ID   */
#define IDS_P065DESC066 (IDS_P065DESC001 + 65)  /* Description Resource ID   */
#define IDS_P065DESC067 (IDS_P065DESC001 + 66)  /* Description Resource ID   */
#define IDS_P065DESC068 (IDS_P065DESC001 + 67)  /* Description Resource ID   */
#define IDS_P065DESC069 (IDS_P065DESC001 + 68)  /* Description Resource ID   */
#define IDS_P065DESC070 (IDS_P065DESC001 + 69)  /* Description Resource ID   */
#define IDS_P065DESC071 (IDS_P065DESC001 + 70)  /* Description Resource ID   */
#define IDS_P065DESC072 (IDS_P065DESC001 + 71)  /* Description Resource ID   */
#define IDS_P065DESC073 (IDS_P065DESC001 + 72)  /* Description Resource ID   */
#define IDS_P065DESC074 (IDS_P065DESC001 + 73)  /* Description Resource ID   */
#define IDS_P065DESC075 (IDS_P065DESC001 + 74)  /* Description Resource ID   */
#define IDS_P065DESC076 (IDS_P065DESC001 + 75)  /* Description Resource ID   */
#define IDS_P065DESC077 (IDS_P065DESC001 + 76)  /* Description Resource ID   */
#define IDS_P065DESC078 (IDS_P065DESC001 + 77)  /* Description Resource ID   */
#define IDS_P065DESC079 (IDS_P065DESC001 + 78)  /* Description Resource ID   */
#define IDS_P065DESC080 (IDS_P065DESC001 + 79)  /* Description Resource ID   */
#define IDS_P065DESC081 (IDS_P065DESC001 + 80)  /* Description Resource ID   */
#define IDS_P065DESC082 (IDS_P065DESC001 + 81)  /* Description Resource ID   */
#define IDS_P065DESC083 (IDS_P065DESC001 + 82)  /* Description Resource ID   */
#define IDS_P065DESC084 (IDS_P065DESC001 + 83)  /* Description Resource ID   */
#define IDS_P065DESC085 (IDS_P065DESC001 + 84)  /* Description Resource ID   */
#define IDS_P065DESC086 (IDS_P065DESC001 + 85)  /* Description Resource ID   */
#define IDS_P065DESC087 (IDS_P065DESC001 + 86)  /* Description Resource ID   */
#define IDS_P065DESC088 (IDS_P065DESC001 + 87)  /* Description Resource ID   */
#define IDS_P065DESC089 (IDS_P065DESC001 + 88)  /* Description Resource ID   */
#define IDS_P065DESC090 (IDS_P065DESC001 + 89)  /* Description Resource ID   */
#define IDS_P065DESC091 (IDS_P065DESC001 + 90)  /* Description Resource ID   */
#define IDS_P065DESC092 (IDS_P065DESC001 + 91)  /* Description Resource ID   */
#define IDS_P065DESC093 (IDS_P065DESC001 + 92)  /* Description Resource ID   */
#define IDS_P065DESC094 (IDS_P065DESC001 + 93)  /* Description Resource ID   */
#define IDS_P065DESC095 (IDS_P065DESC001 + 94)  /* Description Resource ID   */
#define IDS_P065DESC096 (IDS_P065DESC001 + 95)  /* Description Resource ID   */
#define IDS_P065DESC097 (IDS_P065DESC001 + 96)  /* Description Resource ID   */
#define IDS_P065DESC098 (IDS_P065DESC001 + 97)  /* Description Resource ID   */
#define IDS_P065DESC099 (IDS_P065DESC001 + 98)  /* Description Resource ID   */
#define IDS_P065DESC100 (IDS_P065DESC001 + 99)  /* Description Resource ID   */
#define IDS_P065DESC101 (IDS_P065DESC001 +100)  /* Description Resource ID   */
#define IDS_P065DESC102 (IDS_P065DESC001 +101)  /* Description Resource ID   */
#define IDS_P065DESC103 (IDS_P065DESC001 +102)  /* Description Resource ID   */
#define IDS_P065DESC104 (IDS_P065DESC001 +103)  /* Description Resource ID   */
#define IDS_P065DESC105 (IDS_P065DESC001 +104)  /* Description Resource ID   */
#define IDS_P065DESC106 (IDS_P065DESC001 +105)  /* Description Resource ID   */
#define IDS_P065DESC107 (IDS_P065DESC001 +106)  /* Description Resource ID   */
#define IDS_P065DESC108 (IDS_P065DESC001 +107)  /* Description Resource ID   */
#define IDS_P065DESC109 (IDS_P065DESC001 +108)  /* Description Resource ID   */
#define IDS_P065DESC110 (IDS_P065DESC001 +109)  /* Description Resource ID   */
#define IDS_P065DESC111 (IDS_P065DESC001 +110)  /* Description Resource ID   */
#define IDS_P065DESC112 (IDS_P065DESC001 +111)  /* Description Resource ID   */
#define IDS_P065DESC113 (IDS_P065DESC001 +112)  /* Description Resource ID   */
#define IDS_P065DESC114 (IDS_P065DESC001 +113)  /* Description Resource ID   */
#define IDS_P065DESC115 (IDS_P065DESC001 +114)  /* Description Resource ID   */
#define IDS_P065DESC116 (IDS_P065DESC001 +115)  /* Description Resource ID   */
#define IDS_P065DESC117 (IDS_P065DESC001 +116)  /* Description Resource ID   */
#define IDS_P065DESC118 (IDS_P065DESC001 +117)  /* Description Resource ID   */
#define IDS_P065DESC119 (IDS_P065DESC001 +118)  /* Description Resource ID   */
#define IDS_P065DESC120 (IDS_P065DESC001 +119)  /* Description Resource ID   */
#define IDS_P065DESC121 (IDS_P065DESC001 +120)  /* Description Resource ID   */
#define IDS_P065DESC122 (IDS_P065DESC001 +121)  /* Description Resource ID   */
#define IDS_P065DESC123 (IDS_P065DESC001 +122)  /* Description Resource ID   */
#define IDS_P065DESC124 (IDS_P065DESC001 +123)  /* Description Resource ID   */
#define IDS_P065DESC125 (IDS_P065DESC001 +124)  /* Description Resource ID   */
#define IDS_P065DESC126 (IDS_P065DESC001 +125)  /* Description Resource ID   */
#define IDS_P065DESC127 (IDS_P065DESC001 +126)  /* Description Resource ID   */
#define IDS_P065DESC128 (IDS_P065DESC001 +127)  /* Description Resource ID   */
#define IDS_P065DESC129 (IDS_P065DESC001 +128)  /* Description Resource ID   */
#define IDS_P065DESC130 (IDS_P065DESC001 +129)  /* Description Resource ID   */
#define IDS_P065DESC131 (IDS_P065DESC001 +130)  /* Description Resource ID   */
#define IDS_P065DESC132 (IDS_P065DESC001 +131)  /* Description Resource ID   */
#define IDS_P065DESC133 (IDS_P065DESC001 +132)  /* Description Resource ID   */
#define IDS_P065DESC134 (IDS_P065DESC001 +133)  /* Description Resource ID   */
#define IDS_P065DESC135 (IDS_P065DESC001 +134)  /* Description Resource ID   */
#define IDS_P065DESC136 (IDS_P065DESC001 +135)  /* Description Resource ID   */
#define IDS_P065DESC137 (IDS_P065DESC001 +136)  /* Description Resource ID   */
#define IDS_P065DESC138 (IDS_P065DESC001 +137)  /* Description Resource ID   */
#define IDS_P065DESC139 (IDS_P065DESC001 +138)  /* Description Resource ID   */
#define IDS_P065DESC140 (IDS_P065DESC001 +139)  /* Description Resource ID   */
#define IDS_P065DESC141 (IDS_P065DESC001 +140)  /* Description Resource ID   */
#define IDS_P065DESC142 (IDS_P065DESC001 +141)  /* Description Resource ID   */
#define IDS_P065DESC143 (IDS_P065DESC001 +142)  /* Description Resource ID   */
#define IDS_P065DESC144 (IDS_P065DESC001 +143)  /* Description Resource ID   */
#define IDS_P065DESC145 (IDS_P065DESC001 +144)  /* Description Resource ID   */
#define IDS_P065DESC146 (IDS_P065DESC001 +145)  /* Description Resource ID   */
#define IDS_P065DESC147 (IDS_P065DESC001 +146)  /* Description Resource ID   */
#define IDS_P065DESC148 (IDS_P065DESC001 +147)  /* Description Resource ID   */
#define IDS_P065DESC149 (IDS_P065DESC001 +148)  /* Description Resource ID   */
#define IDS_P065DESC150 (IDS_P065DESC001 +149)  /* Description Resource ID   */
#define IDS_P065DESC151 (IDS_P065DESC001 +150)  /* Description Resource ID   */
#define IDS_P065DESC152 (IDS_P065DESC001 +151)  /* Description Resource ID   */
#define IDS_P065DESC153 (IDS_P065DESC001 +152)  /* Description Resource ID   */
#define IDS_P065DESC154 (IDS_P065DESC001 +153)  /* Description Resource ID   */
#define IDS_P065DESC155 (IDS_P065DESC001 +154)  /* Description Resource ID   */
#define IDS_P065DESC156 (IDS_P065DESC001 +155)  /* Description Resource ID   */
#define IDS_P065DESC157 (IDS_P065DESC001 +156)  /* Description Resource ID   */
#define IDS_P065DESC158 (IDS_P065DESC001 +157)  /* Description Resource ID   */
#define IDS_P065DESC159 (IDS_P065DESC001 +158)  /* Description Resource ID   */
#define IDS_P065DESC160 (IDS_P065DESC001 +159)  /* Description Resource ID   */
#define IDS_P065DESC161 (IDS_P065DESC001 +160)  /* Description Resource ID   */
#define IDS_P065DESC162 (IDS_P065DESC001 +161)  /* Description Resource ID   */
#define IDS_P065DESC163 (IDS_P065DESC001 +162)  /* Description Resource ID   */
#define IDS_P065DESC164 (IDS_P065DESC001 +163)  /* Description Resource ID   */
#define IDS_P065DESC165 (IDS_P065DESC001 +164)  /* Description Resource ID   */
#define IDS_P065DESC166 (IDS_P065DESC001 +165)  /* Description Resource ID   */
#define IDS_P065DESC167 (IDS_P065DESC001 +166)  /* Description Resource ID   */
#define IDS_P065DESC168 (IDS_P065DESC001 +167)  /* Description Resource ID   */
#define IDS_P065DESC169 (IDS_P065DESC001 +168)  /* Description Resource ID   */
#define IDS_P065DESC170 (IDS_P065DESC001 +169)  /* Description Resource ID   */
#define IDS_P065DESC171 (IDS_P065DESC001 +170)  /* Description Resource ID   */
#define IDS_P065DESC172 (IDS_P065DESC001 +171)  /* Description Resource ID   */
#define IDS_P065DESC173 (IDS_P065DESC001 +172)  /* Description Resource ID   */
#define IDS_P065DESC174 (IDS_P065DESC001 +173)  /* Description Resource ID   */
#define IDS_P065DESC175 (IDS_P065DESC001 +174)  /* Description Resource ID   */
#define IDS_P065DESC176 (IDS_P065DESC001 +175)  /* Description Resource ID   */
#define IDS_P065DESC177 (IDS_P065DESC001 +176)  /* Description Resource ID   */
#define IDS_P065DESC178 (IDS_P065DESC001 +177)  /* Description Resource ID   */
#define IDS_P065DESC179 (IDS_P065DESC001 +178)  /* Description Resource ID   */
#define IDS_P065DESC180 (IDS_P065DESC001 +179)  /* Description Resource ID   */
#define IDS_P065DESC181 (IDS_P065DESC001 +180)  /* Description Resource ID   */
#define IDS_P065DESC182 (IDS_P065DESC001 +181)  /* Description Resource ID   */
#define IDS_P065DESC183 (IDS_P065DESC001 +182)  /* Description Resource ID   */
#define IDS_P065DESC184 (IDS_P065DESC001 +183)  /* Description Resource ID   */
#define IDS_P065DESC185 (IDS_P065DESC001 +184)  /* Description Resource ID   */
#define IDS_P065DESC186 (IDS_P065DESC001 +185)  /* Description Resource ID   */
#define IDS_P065DESC187 (IDS_P065DESC001 +186)  /* Description Resource ID   */
#define IDS_P065DESC188 (IDS_P065DESC001 +187)  /* Description Resource ID   */
#define IDS_P065DESC189 (IDS_P065DESC001 +188)  /* Description Resource ID   */

/*** NCR2172 ***/
#define IDS_P065DESC190 (IDS_P065DESC001 +189)  /* Description Resource ID   */
#define IDS_P065DESC191 (IDS_P065DESC001 +190)  /* Description Resource ID   */
#define IDS_P065DESC192 (IDS_P065DESC001 +191)  /* Description Resource ID   */
#define IDS_P065DESC193 (IDS_P065DESC001 +192)  /* Description Resource ID   */
#define IDS_P065DESC194 (IDS_P065DESC001 +193)  /* Description Resource ID   */
#define IDS_P065DESC195 (IDS_P065DESC001 +194)  /* Description Resource ID   */
#define IDS_P065DESC196 (IDS_P065DESC001 +195)  /* Description Resource ID   */
#define IDS_P065DESC197 (IDS_P065DESC001 +196)  /* Description Resource ID   */
#define IDS_P065DESC198 (IDS_P065DESC001 +197)  /* Description Resource ID   */
#define IDS_P065DESC199 (IDS_P065DESC001 +198)  /* Description Resource ID   */
#define IDS_P065DESC200 (IDS_P065DESC001 +199)  /* Description Resource ID   */
#define IDS_P065DESC201 (IDS_P065DESC001 +200)  /* Description Resource ID   */
#define IDS_P065DESC202 (IDS_P065DESC001 +201)  /* Description Resource ID   */
#define IDS_P065DESC203 (IDS_P065DESC001 +202)  /* Description Resource ID   */
#define IDS_P065DESC204 (IDS_P065DESC001 +203)  /* Description Resource ID   */
#define IDS_P065DESC205 (IDS_P065DESC001 +204)  /* Description Resource ID   */
#define IDS_P065DESC206 (IDS_P065DESC001 +205)  /* Description Resource ID   */
#define IDS_P065DESC207 (IDS_P065DESC001 +206)  /* Description Resource ID   */
#define IDS_P065DESC208 (IDS_P065DESC001 +207)  /* Description Resource ID   */
#define IDS_P065DESC209 (IDS_P065DESC001 +208)  /* Description Resource ID   */
#define IDS_P065DESC210 (IDS_P065DESC001 +209)  /* Description Resource ID   */
#define IDS_P065DESC211 (IDS_P065DESC001 +210)  /* Description Resource ID   */
#define IDS_P065DESC212 (IDS_P065DESC001 +211)  /* Description Resource ID   */
#define IDS_P065DESC213 (IDS_P065DESC001 +212)  /* Description Resource ID   */
#define IDS_P065DESC214 (IDS_P065DESC001 +213)  /* Description Resource ID   */
#define IDS_P065DESC215 (IDS_P065DESC001 +214)  /* Description Resource ID   */
#define IDS_P065DESC216 (IDS_P065DESC001 +215)  /* Description Resource ID   */
#define IDS_P065DESC217 (IDS_P065DESC001 +216)  /* Description Resource ID   */
#define IDS_P065DESC218 (IDS_P065DESC001 +217)  /* Description Resource ID   */
#define IDS_P065DESC219 (IDS_P065DESC001 +218)  /* Description Resource ID   */
#define IDS_P065DESC220 (IDS_P065DESC001 +219)  /* Description Resource ID   */
#define IDS_P065DESC221 (IDS_P065DESC001 +220)  /* Description Resource ID   */
#define IDS_P065DESC222 (IDS_P065DESC001 +221)  /* Description Resource ID   */
#define IDS_P065DESC223 (IDS_P065DESC001 +222)  /* Description Resource ID   */
#define IDS_P065DESC224 (IDS_P065DESC001 +223)  /* Description Resource ID   */
#define IDS_P065DESC225 (IDS_P065DESC001 +224)  /* Description Resource ID   */
#define IDS_P065DESC226 (IDS_P065DESC001 +225)  /* Description Resource ID   */
#define IDS_P065DESC227 (IDS_P065DESC001 +226)  /* Description Resource ID   */
#define IDS_P065DESC228 (IDS_P065DESC001 +227)  /* Description Resource ID   */
#define IDS_P065DESC229 (IDS_P065DESC001 +228)  /* Description Resource ID   */
#define IDS_P065DESC230 (IDS_P065DESC001 +229)  /* Description Resource ID   */
#define IDS_P065DESC231 (IDS_P065DESC001 +230)  /* Description Resource ID   */
#define IDS_P065DESC232 (IDS_P065DESC001 +231)  /* Description Resource ID   */
#define IDS_P065DESC233 (IDS_P065DESC001 +232)  /* Description Resource ID   */
#define IDS_P065DESC234 (IDS_P065DESC001 +233)  /* Description Resource ID   */
#define IDS_P065DESC235 (IDS_P065DESC001 +234)  /* Description Resource ID   */
#define IDS_P065DESC236 (IDS_P065DESC001 +235)  /* Description Resource ID   */
#define IDS_P065DESC237 (IDS_P065DESC001 +236)  /* Description Resource ID   */
#define IDS_P065DESC238 (IDS_P065DESC001 +237)  /* Description Resource ID   */
#define IDS_P065DESC239 (IDS_P065DESC001 +238)  /* Description Resource ID   */
#define IDS_P065DESC240 (IDS_P065DESC001 +239)  /* Description Resource ID   */

/* #define IDS_P065_ERRUNKNOWN (IDS_P065DESC001 + 200)  Unknown Error Message */
#define IDS_P065_ERRUNKNOWN (IDS_P065DESC001 + 240) /* Unknown Error Message */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P65_ADDR_MAX    MAX_MLDMNEMO_SIZE       /* Number of Address with     */
                                                /*              Reserved Area */

#define P65_ADDR_NO     MAX_MLDMNEMO_SIZE             /* Number of Address without  */
                                                /*              Reserved Area */

#define P65_MNE_LEN     32                      /* Max Length of Mnemonic     */

//6/29/04 - increased max description length to accomodate for french and spanish mnemonics
#define P65_DESC_LEN    /*50*/100                     /* Max Length of Description  */

#define P65_ERR_LEN     PEP_ALLOC_LEN           /* Max Length of Error Message */
#define P65_CAP_LEN     PEP_CAPTION_LEN         /* Length of Messgae Caption   */

#define P65_PAGE_MAX    10                      /* No. of Items in One Page     */
#define P65_ONE_LINE    1                       /* Scrool to One Line Up / Down */
#define P65_ONE_PAGE    (P65_PAGE_MAX - P65_ONE_LINE)
                                                /* Scrool to One Page Up / Down */

#define P65_SCBAR_MIN   0                       /* Min. Value of Scrool Range   */
#define P65_SCBAR_MAX   (MAX_MLDMNEMO_SIZE - P65_PAGE_MAX) /* Max. Value of Scrool Range */

#define P65_MNE_LEN_MIN 0                       /* Min Length of Mnemonic       */


/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/

#define P65GETINDEX(id, top)    (((id) - IDD_P65_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/

typedef struct _P65WORK {
    MLDIF   MLDWork[P65_ADDR_MAX];      /* MLD File Work Data Area*/
} P65MLDWORK, FAR* LPP65MLDWORK;

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
BOOL    P65InitDlg(HWND);
VOID    P65GetMnemo(HWND, WORD wItem);
VOID    P65DispAllocErr(HWND, HGLOBAL);
BOOL    P65ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P65CalcIndex(WORD wScrlCode);
VOID    P65RedrawText(HWND);
void    DispError(HWND, LPCWSTR);

/* ===== End of File (P065.H) ===== */
