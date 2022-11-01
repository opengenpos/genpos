/*
* ---------------------------------------------------------------------------
* Title         :   Multi-Line Display Mnemonics Header File (Prog. 65)
* Category      :   Setup, NCR 2170 PEP for Windows (Hotel US Model)
* Program Name  :   P072.H
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
#define IDD_P72_SCBAR   26501

#define IDD_P72_DESC01  (IDD_P72_SCBAR +  1)
#define IDD_P72_DESC02  (IDD_P72_SCBAR +  2)
#define IDD_P72_DESC03  (IDD_P72_SCBAR +  3)
#define IDD_P72_DESC04  (IDD_P72_SCBAR +  4)
#define IDD_P72_DESC05  (IDD_P72_SCBAR +  5)
#define IDD_P72_DESC06  (IDD_P72_SCBAR +  6)
#define IDD_P72_DESC07  (IDD_P72_SCBAR +  7)
#define IDD_P72_DESC08  (IDD_P72_SCBAR +  8)
#define IDD_P72_DESC09  (IDD_P72_SCBAR +  9)
#define IDD_P72_DESC10  (IDD_P72_SCBAR + 10)

#define IDD_P72_EDIT01  (IDD_P72_SCBAR + 11)
#define IDD_P72_EDIT02  (IDD_P72_SCBAR + 12)
#define IDD_P72_EDIT03  (IDD_P72_SCBAR + 13)
#define IDD_P72_EDIT04  (IDD_P72_SCBAR + 14)
#define IDD_P72_EDIT05  (IDD_P72_SCBAR + 15)
#define IDD_P72_EDIT06  (IDD_P72_SCBAR + 16)
#define IDD_P72_EDIT07  (IDD_P72_SCBAR + 17)
#define IDD_P72_EDIT08  (IDD_P72_SCBAR + 18)
#define IDD_P72_EDIT09  (IDD_P72_SCBAR + 19)
#define IDD_P72_EDIT10  (IDD_P72_SCBAR + 20)

#define IDD_P72_CAPTION1	26525
#define	IDD_P72_ADDR		26526
#define	IDD_P72_DESC		26527
#define IDD_P72_MNEM		26528

#define IDD_P72_LABEL_01      26529
#define IDD_P72_01_1          (IDD_P72_LABEL_01+1)
#define IDD_P72_01_2          (IDD_P72_LABEL_01+2)
#define IDD_P72_SPIN01_1      (IDD_P72_LABEL_01+3)
#define IDD_P72_SPIN01_2      (IDD_P72_LABEL_01+4)
#define IDD_P72_LABEL_02      (IDD_P72_SPIN01_2+1)
#define IDD_P72_02_1          (IDD_P72_LABEL_02+1)
#define IDD_P72_02_2          (IDD_P72_LABEL_02+2)
#define IDD_P72_SPIN02_1      (IDD_P72_LABEL_02+3)
#define IDD_P72_SPIN02_2      (IDD_P72_LABEL_02+4)
#define IDD_P72_LABEL_03      (IDD_P72_SPIN02_2+1)
#define IDD_P72_03_1          (IDD_P72_LABEL_03+1)
#define IDD_P72_03_2          (IDD_P72_LABEL_03+2)
#define IDD_P72_SPIN03_1      (IDD_P72_LABEL_03+3)
#define IDD_P72_SPIN03_2      (IDD_P72_LABEL_03+4)
#define IDD_P72_LABEL_04      (IDD_P72_SPIN03_2+1)
#define IDD_P72_04_1          (IDD_P72_LABEL_04+1)
#define IDD_P72_04_2          (IDD_P72_LABEL_04+2)
#define IDD_P72_SPIN04_1      (IDD_P72_LABEL_04+3)
#define IDD_P72_SPIN04_2      (IDD_P72_LABEL_04+4)
#define IDD_P72_LABEL_05      (IDD_P72_SPIN04_2+1)
#define IDD_P72_05_1          (IDD_P72_LABEL_05+1)
#define IDD_P72_05_2          (IDD_P72_LABEL_05+2)
#define IDD_P72_SPIN05_1      (IDD_P72_LABEL_05+3)
#define IDD_P72_SPIN05_2      (IDD_P72_LABEL_05+4)
#define IDD_P72_LABEL_06      (IDD_P72_SPIN05_2+1)
#define IDD_P72_06_1          (IDD_P72_LABEL_06+1)
#define IDD_P72_06_2          (IDD_P72_LABEL_06+2)
#define IDD_P72_SPIN06_1      (IDD_P72_LABEL_06+3)
#define IDD_P72_SPIN06_2      (IDD_P72_LABEL_06+4)

/*
* ===========================================================================
*       Strings ID
* ===========================================================================
*/
#define IDS_P072DESC001 IDS_P72                 /* Description Resource ID   */
#define IDS_P072DESC002 (IDS_P072DESC001 +  1)  /* Description Resource ID   */
#define IDS_P072DESC003 (IDS_P072DESC001 +  2)  /* Description Resource ID   */
#define IDS_P072DESC004 (IDS_P072DESC001 +  3)  /* Description Resource ID   */
#define IDS_P072DESC005 (IDS_P072DESC001 +  4)  /* Description Resource ID   */
#define IDS_P072DESC006 (IDS_P072DESC001 +  5)  /* Description Resource ID   */
#define IDS_P072DESC007 (IDS_P072DESC001 +  6)  /* Description Resource ID   */
#define IDS_P072DESC008 (IDS_P072DESC001 +  7)  /* Description Resource ID   */
#define IDS_P072DESC009 (IDS_P072DESC001 +  8)  /* Description Resource ID   */
#define IDS_P072DESC010 (IDS_P072DESC001 +  9)  /* Description Resource ID   */
#define IDS_P072DESC011 (IDS_P072DESC001 + 10)  /* Description Resource ID   */
#define IDS_P072DESC012 (IDS_P072DESC001 + 11)  /* Description Resource ID   */
#define IDS_P072DESC013 (IDS_P072DESC001 + 12)  /* Description Resource ID   */
#define IDS_P072DESC014 (IDS_P072DESC001 + 13)  /* Description Resource ID   */
#define IDS_P072DESC015 (IDS_P072DESC001 + 14)  /* Description Resource ID   */
#define IDS_P072DESC016 (IDS_P072DESC001 + 15)  /* Description Resource ID   */
#define IDS_P072DESC017 (IDS_P072DESC001 + 16)  /* Description Resource ID   */
#define IDS_P072DESC018 (IDS_P072DESC001 + 17)  /* Description Resource ID   */
#define IDS_P072DESC019 (IDS_P072DESC001 + 18)  /* Description Resource ID   */
#define IDS_P072DESC020 (IDS_P072DESC001 + 19)  /* Description Resource ID   */
#define IDS_P072DESC021 (IDS_P072DESC001 + 20)  /* Description Resource ID   */
#define IDS_P072DESC022 (IDS_P072DESC001 + 21)  /* Description Resource ID   */
#define IDS_P072DESC023 (IDS_P072DESC001 + 22)  /* Description Resource ID   */
#define IDS_P072DESC024 (IDS_P072DESC001 + 23)  /* Description Resource ID   */
#define IDS_P072DESC025 (IDS_P072DESC001 + 24)  /* Description Resource ID   */
#define IDS_P072DESC026 (IDS_P072DESC001 + 25)  /* Description Resource ID   */
#define IDS_P072DESC027 (IDS_P072DESC001 + 26)  /* Description Resource ID   */
#define IDS_P072DESC028 (IDS_P072DESC001 + 27)  /* Description Resource ID   */
#define IDS_P072DESC029 (IDS_P072DESC001 + 28)  /* Description Resource ID   */
#define IDS_P072DESC030 (IDS_P072DESC001 + 29)  /* Description Resource ID   */
#define IDS_P072DESC031 (IDS_P072DESC001 + 30)  /* Description Resource ID   */
#define IDS_P072DESC032 (IDS_P072DESC001 + 31)  /* Description Resource ID   */
#define IDS_P072DESC033 (IDS_P072DESC001 + 32)  /* Description Resource ID   */
#define IDS_P072DESC034 (IDS_P072DESC001 + 33)  /* Description Resource ID   */
#define IDS_P072DESC035 (IDS_P072DESC001 + 34)  /* Description Resource ID   */
#define IDS_P072DESC036 (IDS_P072DESC001 + 35)  /* Description Resource ID   */
#define IDS_P072DESC037 (IDS_P072DESC001 + 36)  /* Description Resource ID   */
#define IDS_P072DESC038 (IDS_P072DESC001 + 37)  /* Description Resource ID   */
#define IDS_P072DESC039 (IDS_P072DESC001 + 38)  /* Description Resource ID   */
#define IDS_P072DESC040 (IDS_P072DESC001 + 39)  /* Description Resource ID   */
#define IDS_P072DESC041 (IDS_P072DESC001 + 40)  /* Description Resource ID   */
#define IDS_P072DESC042 (IDS_P072DESC001 + 41)  /* Description Resource ID   */
#define IDS_P072DESC043 (IDS_P072DESC001 + 42)  /* Description Resource ID   */
#define IDS_P072DESC044 (IDS_P072DESC001 + 43)  /* Description Resource ID   */
#define IDS_P072DESC045 (IDS_P072DESC001 + 44)  /* Description Resource ID   */
#define IDS_P072DESC046 (IDS_P072DESC001 + 45)  /* Description Resource ID   */
#define IDS_P072DESC047 (IDS_P072DESC001 + 46)  /* Description Resource ID   */
#define IDS_P072DESC048 (IDS_P072DESC001 + 47)  /* Description Resource ID   */
#define IDS_P072DESC049 (IDS_P072DESC001 + 48)  /* Description Resource ID   */
#define IDS_P072DESC050 (IDS_P072DESC001 + 49)  /* Description Resource ID   */
#define IDS_P072DESC051 (IDS_P072DESC001 + 50)  /* Description Resource ID   */
#define IDS_P072DESC052 (IDS_P072DESC001 + 51)  /* Description Resource ID   */
#define IDS_P072DESC053 (IDS_P072DESC001 + 52)  /* Description Resource ID   */
#define IDS_P072DESC054 (IDS_P072DESC001 + 53)  /* Description Resource ID   */
#define IDS_P072DESC055 (IDS_P072DESC001 + 54)  /* Description Resource ID   */
#define IDS_P072DESC056 (IDS_P072DESC001 + 55)  /* Description Resource ID   */
#define IDS_P072DESC057 (IDS_P072DESC001 + 56)  /* Description Resource ID   */
#define IDS_P072DESC058 (IDS_P072DESC001 + 57)  /* Description Resource ID   */
#define IDS_P072DESC059 (IDS_P072DESC001 + 58)  /* Description Resource ID   */
#define IDS_P072DESC060 (IDS_P072DESC001 + 59)  /* Description Resource ID   */
#define IDS_P072DESC061 (IDS_P072DESC001 + 60)  /* Description Resource ID   */
#define IDS_P072DESC062 (IDS_P072DESC001 + 61)  /* Description Resource ID   */
#define IDS_P072DESC063 (IDS_P072DESC001 + 62)  /* Description Resource ID   */
#define IDS_P072DESC064 (IDS_P072DESC001 + 63)  /* Description Resource ID   */
#define IDS_P072DESC065 (IDS_P072DESC001 + 64)  /* Description Resource ID   */
#define IDS_P072DESC066 (IDS_P072DESC001 + 65)  /* Description Resource ID   */
#define IDS_P072DESC067 (IDS_P072DESC001 + 66)  /* Description Resource ID   */
#define IDS_P072DESC068 (IDS_P072DESC001 + 67)  /* Description Resource ID   */
#define IDS_P072DESC069 (IDS_P072DESC001 + 68)  /* Description Resource ID   */
#define IDS_P072DESC070 (IDS_P072DESC001 + 69)  /* Description Resource ID   */
#define IDS_P072DESC071 (IDS_P072DESC001 + 70)  /* Description Resource ID   */
#define IDS_P072DESC072 (IDS_P072DESC001 + 71)  /* Description Resource ID   */
#define IDS_P072DESC073 (IDS_P072DESC001 + 72)  /* Description Resource ID   */
#define IDS_P072DESC074 (IDS_P072DESC001 + 73)  /* Description Resource ID   */
#define IDS_P072DESC075 (IDS_P072DESC001 + 74)  /* Description Resource ID   */
#define IDS_P072DESC076 (IDS_P072DESC001 + 75)  /* Description Resource ID   */
#define IDS_P072DESC077 (IDS_P072DESC001 + 76)  /* Description Resource ID   */
#define IDS_P072DESC078 (IDS_P072DESC001 + 77)  /* Description Resource ID   */
#define IDS_P072DESC079 (IDS_P072DESC001 + 78)  /* Description Resource ID   */
#define IDS_P072DESC080 (IDS_P072DESC001 + 79)  /* Description Resource ID   */
#define IDS_P072DESC081 (IDS_P072DESC001 + 80)  /* Description Resource ID   */
#define IDS_P072DESC082 (IDS_P072DESC001 + 81)  /* Description Resource ID   */
#define IDS_P072DESC083 (IDS_P072DESC001 + 82)  /* Description Resource ID   */
#define IDS_P072DESC084 (IDS_P072DESC001 + 83)  /* Description Resource ID   */
#define IDS_P072DESC085 (IDS_P072DESC001 + 84)  /* Description Resource ID   */
#define IDS_P072DESC086 (IDS_P072DESC001 + 85)  /* Description Resource ID   */
#define IDS_P072DESC087 (IDS_P072DESC001 + 86)  /* Description Resource ID   */
#define IDS_P072DESC088 (IDS_P072DESC001 + 87)  /* Description Resource ID   */
#define IDS_P072DESC089 (IDS_P072DESC001 + 88)  /* Description Resource ID   */
#define IDS_P072DESC090 (IDS_P072DESC001 + 89)  /* Description Resource ID   */
#define IDS_P072DESC091 (IDS_P072DESC001 + 90)  /* Description Resource ID   */
#define IDS_P072DESC092 (IDS_P072DESC001 + 91)  /* Description Resource ID   */
#define IDS_P072DESC093 (IDS_P072DESC001 + 92)  /* Description Resource ID   */
#define IDS_P072DESC094 (IDS_P072DESC001 + 93)  /* Description Resource ID   */
#define IDS_P072DESC095 (IDS_P072DESC001 + 94)  /* Description Resource ID   */
#define IDS_P072DESC096 (IDS_P072DESC001 + 95)  /* Description Resource ID   */
#define IDS_P072DESC097 (IDS_P072DESC001 + 96)  /* Description Resource ID   */
#define IDS_P072DESC098 (IDS_P072DESC001 + 97)  /* Description Resource ID   */
#define IDS_P072DESC099 (IDS_P072DESC001 + 98)  /* Description Resource ID   */
#define IDS_P072DESC100 (IDS_P072DESC001 + 99)  /* Description Resource ID   */
#define IDS_P072DESC101 (IDS_P072DESC001 +100)  /* Description Resource ID   */
#define IDS_P072DESC102 (IDS_P072DESC001 +101)  /* Description Resource ID   */
#define IDS_P072DESC103 (IDS_P072DESC001 +102)  /* Description Resource ID   */
#define IDS_P072DESC104 (IDS_P072DESC001 +103)  /* Description Resource ID   */
#define IDS_P072DESC105 (IDS_P072DESC001 +104)  /* Description Resource ID   */
#define IDS_P072DESC106 (IDS_P072DESC001 +105)  /* Description Resource ID   */
#define IDS_P072DESC107 (IDS_P072DESC001 +106)  /* Description Resource ID   */
#define IDS_P072DESC108 (IDS_P072DESC001 +107)  /* Description Resource ID   */
#define IDS_P072DESC109 (IDS_P072DESC001 +108)  /* Description Resource ID   */
#define IDS_P072DESC110 (IDS_P072DESC001 +109)  /* Description Resource ID   */
#define IDS_P072DESC111 (IDS_P072DESC001 +110)  /* Description Resource ID   */
#define IDS_P072DESC112 (IDS_P072DESC001 +111)  /* Description Resource ID   */
#define IDS_P072DESC113 (IDS_P072DESC001 +112)  /* Description Resource ID   */
#define IDS_P072DESC114 (IDS_P072DESC001 +113)  /* Description Resource ID   */
#define IDS_P072DESC115 (IDS_P072DESC001 +114)  /* Description Resource ID   */
#define IDS_P072DESC116 (IDS_P072DESC001 +115)  /* Description Resource ID   */
#define IDS_P072DESC117 (IDS_P072DESC001 +116)  /* Description Resource ID   */
#define IDS_P072DESC118 (IDS_P072DESC001 +117)  /* Description Resource ID   */
#define IDS_P072DESC119 (IDS_P072DESC001 +118)  /* Description Resource ID   */
#define IDS_P072DESC120 (IDS_P072DESC001 +119)  /* Description Resource ID   */
#define IDS_P072DESC121 (IDS_P072DESC001 +120)  /* Description Resource ID   */
#define IDS_P072DESC122 (IDS_P072DESC001 +121)  /* Description Resource ID   */
#define IDS_P072DESC123 (IDS_P072DESC001 +122)  /* Description Resource ID   */
#define IDS_P072DESC124 (IDS_P072DESC001 +123)  /* Description Resource ID   */
#define IDS_P072DESC125 (IDS_P072DESC001 +124)  /* Description Resource ID   */
#define IDS_P072DESC126 (IDS_P072DESC001 +125)  /* Description Resource ID   */
#define IDS_P072DESC127 (IDS_P072DESC001 +126)  /* Description Resource ID   */
#define IDS_P072DESC128 (IDS_P072DESC001 +127)  /* Description Resource ID   */
#define IDS_P072DESC129 (IDS_P072DESC001 +128)  /* Description Resource ID   */
#define IDS_P072DESC130 (IDS_P072DESC001 +129)  /* Description Resource ID   */
#define IDS_P072DESC131 (IDS_P072DESC001 +130)  /* Description Resource ID   */
#define IDS_P072DESC132 (IDS_P072DESC001 +131)  /* Description Resource ID   */
#define IDS_P072DESC133 (IDS_P072DESC001 +132)  /* Description Resource ID   */
#define IDS_P072DESC134 (IDS_P072DESC001 +133)  /* Description Resource ID   */
#define IDS_P072DESC135 (IDS_P072DESC001 +134)  /* Description Resource ID   */
#define IDS_P072DESC136 (IDS_P072DESC001 +135)  /* Description Resource ID   */
#define IDS_P072DESC137 (IDS_P072DESC001 +136)  /* Description Resource ID   */
#define IDS_P072DESC138 (IDS_P072DESC001 +137)  /* Description Resource ID   */
#define IDS_P072DESC139 (IDS_P072DESC001 +138)  /* Description Resource ID   */
#define IDS_P072DESC140 (IDS_P072DESC001 +139)  /* Description Resource ID   */
#define IDS_P072DESC141 (IDS_P072DESC001 +140)  /* Description Resource ID   */
#define IDS_P072DESC142 (IDS_P072DESC001 +141)  /* Description Resource ID   */
#define IDS_P072DESC143 (IDS_P072DESC001 +142)  /* Description Resource ID   */
#define IDS_P072DESC144 (IDS_P072DESC001 +143)  /* Description Resource ID   */
#define IDS_P072DESC145 (IDS_P072DESC001 +144)  /* Description Resource ID   */
#define IDS_P072DESC146 (IDS_P072DESC001 +145)  /* Description Resource ID   */
#define IDS_P072DESC147 (IDS_P072DESC001 +146)  /* Description Resource ID   */
#define IDS_P072DESC148 (IDS_P072DESC001 +147)  /* Description Resource ID   */
#define IDS_P072DESC149 (IDS_P072DESC001 +148)  /* Description Resource ID   */
#define IDS_P072DESC150 (IDS_P072DESC001 +149)  /* Description Resource ID   */
#define IDS_P072DESC151 (IDS_P072DESC001 +150)  /* Description Resource ID   */
#define IDS_P072DESC152 (IDS_P072DESC001 +151)  /* Description Resource ID   */
#define IDS_P072DESC153 (IDS_P072DESC001 +152)  /* Description Resource ID   */
#define IDS_P072DESC154 (IDS_P072DESC001 +153)  /* Description Resource ID   */
#define IDS_P072DESC155 (IDS_P072DESC001 +154)  /* Description Resource ID   */
#define IDS_P072DESC156 (IDS_P072DESC001 +155)  /* Description Resource ID   */
#define IDS_P072DESC157 (IDS_P072DESC001 +156)  /* Description Resource ID   */
#define IDS_P072DESC158 (IDS_P072DESC001 +157)  /* Description Resource ID   */
#define IDS_P072DESC159 (IDS_P072DESC001 +158)  /* Description Resource ID   */
#define IDS_P072DESC160 (IDS_P072DESC001 +159)  /* Description Resource ID   */
#define IDS_P072DESC161 (IDS_P072DESC001 +160)  /* Description Resource ID   */
#define IDS_P072DESC162 (IDS_P072DESC001 +161)  /* Description Resource ID   */
#define IDS_P072DESC163 (IDS_P072DESC001 +162)  /* Description Resource ID   */
#define IDS_P072DESC164 (IDS_P072DESC001 +163)  /* Description Resource ID   */
#define IDS_P072DESC165 (IDS_P072DESC001 +164)  /* Description Resource ID   */
#define IDS_P072DESC166 (IDS_P072DESC001 +165)  /* Description Resource ID   */
#define IDS_P072DESC167 (IDS_P072DESC001 +166)  /* Description Resource ID   */
#define IDS_P072DESC168 (IDS_P072DESC001 +167)  /* Description Resource ID   */
#define IDS_P072DESC169 (IDS_P072DESC001 +168)  /* Description Resource ID   */
#define IDS_P072DESC170 (IDS_P072DESC001 +169)  /* Description Resource ID   */
#define IDS_P072DESC171 (IDS_P072DESC001 +170)  /* Description Resource ID   */
#define IDS_P072DESC172 (IDS_P072DESC001 +171)  /* Description Resource ID   */
#define IDS_P072DESC173 (IDS_P072DESC001 +172)  /* Description Resource ID   */
#define IDS_P072DESC174 (IDS_P072DESC001 +173)  /* Description Resource ID   */
#define IDS_P072DESC175 (IDS_P072DESC001 +174)  /* Description Resource ID   */
#define IDS_P072DESC176 (IDS_P072DESC001 +175)  /* Description Resource ID   */
#define IDS_P072DESC177 (IDS_P072DESC001 +176)  /* Description Resource ID   */
#define IDS_P072DESC178 (IDS_P072DESC001 +177)  /* Description Resource ID   */
#define IDS_P072DESC179 (IDS_P072DESC001 +178)  /* Description Resource ID   */
#define IDS_P072DESC180 (IDS_P072DESC001 +179)  /* Description Resource ID   */
#define IDS_P072DESC181 (IDS_P072DESC001 +180)  /* Description Resource ID   */
#define IDS_P072DESC182 (IDS_P072DESC001 +181)  /* Description Resource ID   */
#define IDS_P072DESC183 (IDS_P072DESC001 +182)  /* Description Resource ID   */
#define IDS_P072DESC184 (IDS_P072DESC001 +183)  /* Description Resource ID   */
#define IDS_P072DESC185 (IDS_P072DESC001 +184)  /* Description Resource ID   */
#define IDS_P072DESC186 (IDS_P072DESC001 +185)  /* Description Resource ID   */
#define IDS_P072DESC187 (IDS_P072DESC001 +186)  /* Description Resource ID   */
#define IDS_P072DESC188 (IDS_P072DESC001 +187)  /* Description Resource ID   */
#define IDS_P072DESC189 (IDS_P072DESC001 +188)  /* Description Resource ID   */

/*** NCR2172 ***/
#define IDS_P072DESC190 (IDS_P072DESC001 +189)  /* Description Resource ID   */
#define IDS_P072DESC191 (IDS_P072DESC001 +190)  /* Description Resource ID   */
#define IDS_P072DESC192 (IDS_P072DESC001 +191)  /* Description Resource ID   */
#define IDS_P072DESC193 (IDS_P072DESC001 +192)  /* Description Resource ID   */
#define IDS_P072DESC194 (IDS_P072DESC001 +193)  /* Description Resource ID   */
#define IDS_P072DESC195 (IDS_P072DESC001 +194)  /* Description Resource ID   */
#define IDS_P072DESC196 (IDS_P072DESC001 +195)  /* Description Resource ID   */
#define IDS_P072DESC197 (IDS_P072DESC001 +196)  /* Description Resource ID   */
#define IDS_P072DESC198 (IDS_P072DESC001 +197)  /* Description Resource ID   */
#define IDS_P072DESC199 (IDS_P072DESC001 +198)  /* Description Resource ID   */
#define IDS_P072DESC200 (IDS_P072DESC001 +199)  /* Description Resource ID   */
#define IDS_P072DESC201 (IDS_P072DESC001 +200)  /* Description Resource ID   */
#define IDS_P072DESC202 (IDS_P072DESC001 +201)  /* Description Resource ID   */
#define IDS_P072DESC203 (IDS_P072DESC001 +202)  /* Description Resource ID   */
#define IDS_P072DESC204 (IDS_P072DESC001 +203)  /* Description Resource ID   */
#define IDS_P072DESC205 (IDS_P072DESC001 +204)  /* Description Resource ID   */
#define IDS_P072DESC206 (IDS_P072DESC001 +205)  /* Description Resource ID   */
#define IDS_P072DESC207 (IDS_P072DESC001 +206)  /* Description Resource ID   */
#define IDS_P072DESC208 (IDS_P072DESC001 +207)  /* Description Resource ID   */
#define IDS_P072DESC209 (IDS_P072DESC001 +208)  /* Description Resource ID   */
#define IDS_P072DESC210 (IDS_P072DESC001 +209)  /* Description Resource ID   */
#define IDS_P072DESC211 (IDS_P072DESC001 +210)  /* Description Resource ID   */
#define IDS_P072DESC212 (IDS_P072DESC001 +211)  /* Description Resource ID   */
#define IDS_P072DESC213 (IDS_P072DESC001 +212)  /* Description Resource ID   */
#define IDS_P072DESC214 (IDS_P072DESC001 +213)  /* Description Resource ID   */
#define IDS_P072DESC215 (IDS_P072DESC001 +214)  /* Description Resource ID   */
#define IDS_P072DESC216 (IDS_P072DESC001 +215)  /* Description Resource ID   */
#define IDS_P072DESC217 (IDS_P072DESC001 +216)  /* Description Resource ID   */
#define IDS_P072DESC218 (IDS_P072DESC001 +217)  /* Description Resource ID   */
#define IDS_P072DESC219 (IDS_P072DESC001 +218)  /* Description Resource ID   */
#define IDS_P072DESC220 (IDS_P072DESC001 +219)  /* Description Resource ID   */
#define IDS_P072DESC221 (IDS_P072DESC001 +220)  /* Description Resource ID   */
#define IDS_P072DESC222 (IDS_P072DESC001 +221)  /* Description Resource ID   */
#define IDS_P072DESC223 (IDS_P072DESC001 +222)  /* Description Resource ID   */
#define IDS_P072DESC224 (IDS_P072DESC001 +223)  /* Description Resource ID   */
#define IDS_P072DESC225 (IDS_P072DESC001 +224)  /* Description Resource ID   */
#define IDS_P072DESC226 (IDS_P072DESC001 +225)  /* Description Resource ID   */
#define IDS_P072DESC227 (IDS_P072DESC001 +226)  /* Description Resource ID   */
#define IDS_P072DESC228 (IDS_P072DESC001 +227)  /* Description Resource ID   */
#define IDS_P072DESC229 (IDS_P072DESC001 +228)  /* Description Resource ID   */
#define IDS_P072DESC230 (IDS_P072DESC001 +229)  /* Description Resource ID   */
#define IDS_P072DESC231 (IDS_P072DESC001 +230)  /* Description Resource ID   */
#define IDS_P072DESC232 (IDS_P072DESC001 +231)  /* Description Resource ID   */
#define IDS_P072DESC233 (IDS_P072DESC001 +232)  /* Description Resource ID   */
#define IDS_P072DESC234 (IDS_P072DESC001 +233)  /* Description Resource ID   */
#define IDS_P072DESC235 (IDS_P072DESC001 +234)  /* Description Resource ID   */
#define IDS_P072DESC236 (IDS_P072DESC001 +235)  /* Description Resource ID   */
#define IDS_P072DESC237 (IDS_P072DESC001 +236)  /* Description Resource ID   */
#define IDS_P072DESC238 (IDS_P072DESC001 +237)  /* Description Resource ID   */
#define IDS_P072DESC239 (IDS_P072DESC001 +238)  /* Description Resource ID   */
#define IDS_P072DESC240 (IDS_P072DESC001 +239)  /* Description Resource ID   */

/* #define IDS_P072_ERRUNKNOWN (IDS_P072DESC001 + 200)  Unknown Error Message */
#define IDS_P072_ERRUNKNOWN (IDS_P072DESC001 + 240) /* Unknown Error Message */

/*
* ===========================================================================
*       Local Value
* ===========================================================================
*/
#define P72_ADDR_MAX    MAX_RSNMNEMO_SIZE       /* Number of Address with     */
                                                /*              Reserved Area */

#define P72_ADDR_NO     MAX_RSNMNEMO_SIZE             /* Number of Address without  */
                                                /*              Reserved Area */

#define P72_MNE_LEN     32                      /* Max Length of Mnemonic     */

//6/29/04 - increased max description length to accomodate for french and spanish mnemonics
#define P72_DESC_LEN    /*50*/100                     /* Max Length of Description  */

#define P72_ERR_LEN     PEP_ALLOC_LEN           /* Max Length of Error Message */
#define P72_CAP_LEN     PEP_CAPTION_LEN         /* Length of Messgae Caption   */

#define P72_PAGE_MAX    10                      /* No. of Items in One Page     */
#define P72_ONE_LINE    1                       /* Scrool to One Line Up / Down */
#define P72_ONE_PAGE    (P72_PAGE_MAX - P72_ONE_LINE)
                                                /* Scrool to One Page Up / Down */

#define P72_SCBAR_MIN   0                       /* Min. Value of Scrool Range   */
#define P72_SCBAR_MAX   (MAX_RSNMNEMO_SIZE - P72_PAGE_MAX) /* Max. Value of Scrool Range */

#define P72_MNE_LEN_MIN 0                       /* Min Length of Mnemonic       */


/*
* ===========================================================================
*       Macro Type Define Declarations
* ===========================================================================
*/

#define P72GETINDEX(id, top)    (((id) - IDD_P72_EDIT01) + (top))

/*
* ===========================================================================
*       Structure Type Define Declarations
* ===========================================================================
*/

typedef struct _P72WORK {
    OPMNEMONICFILE   RSNWork[P72_ADDR_MAX];      /* MLD File Work Data Area*/
} P72RSNWORK, * LPP72RSNWORK;

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
BOOL    P72InitDlg(HWND);
VOID    P72GetMnemo(HWND, WORD wItem);
VOID    P72DispAllocErr(HWND, HGLOBAL);
BOOL    P72ScrlProc(HWND, WPARAM, LPARAM);
BOOL    P72CalcIndex(WORD wScrlCode);
VOID    P72RedrawText(HWND);
void    DispError(HWND, LPCWSTR);

/* ===== End of File (P072.H) ===== */
