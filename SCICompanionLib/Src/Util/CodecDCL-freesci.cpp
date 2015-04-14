#include "stdafx.h"
#include "Codec.h"

/***************************************************************************
decompress1.c Copyright (C) 1999 The FreeSCI project
This program may be modified and copied freely according to the terms of
the GNU general public license (GPL), as long as the above copyright
notice and the licensing information contained herein are preserved.
Please refer to www.gnu.org for licensing details.
This work is provided AS IS, without warranty of any kind, expressed or
implied, including but not limited to the warranties of merchantibility,
noninfringement, and fitness for a specific purpose. The author will not
be held liable for any damage caused by this work or derivatives of it.
By using this source code, you agree to the licensing terms as stated
above.
Please contact the maintainer for bug reports or inquiries.
Current Maintainer:
Christoph Reichenbach (CJR) [creichen@rbg.informatik.tu-darmstadt.de]
***************************************************************************/
/* Reads data from a resource file and stores the result in memory */


/* DEFLATE-DCL
** Refer to the FreeSCI docs for a full description.
*/

#define HUFFMAN_LEAF 0x40000000

struct bit_read_struct {
    int length;
    int bitpos;
    int bytepos;
    byte *data;
};

#define BRANCH_SHIFT 12
#define BRANCH_NODE(pos, left, right)  ((left << BRANCH_SHIFT) | (right)),
#define LEAF_NODE(pos, value)  ((value) | HUFFMAN_LEAF),


static int length_tree[] = {
    BRANCH_NODE(0, 1, 2)
    BRANCH_NODE(1, 3, 4)
    BRANCH_NODE(2, 5, 6)
    BRANCH_NODE(3, 7, 8)
    BRANCH_NODE(4, 9, 10)
    BRANCH_NODE(5, 11, 12)
    LEAF_NODE(6, 1)
    BRANCH_NODE(7, 13, 14)
    BRANCH_NODE(8, 15, 16)
    BRANCH_NODE(9, 17, 18)
    LEAF_NODE(10, 3)
    LEAF_NODE(11, 2)
    LEAF_NODE(12, 0)
    BRANCH_NODE(13, 19, 20)
    BRANCH_NODE(14, 21, 22)
    BRANCH_NODE(15, 23, 24)
    LEAF_NODE(16, 6)
    LEAF_NODE(17, 5)
    LEAF_NODE(18, 4)
    BRANCH_NODE(19, 25, 26)
    BRANCH_NODE(20, 27, 28)
    LEAF_NODE(21, 10)
    LEAF_NODE(22, 9)
    LEAF_NODE(23, 8)
    LEAF_NODE(24, 7)
    BRANCH_NODE(25, 29, 30)
    LEAF_NODE(26, 13)
    LEAF_NODE(27, 12)
    LEAF_NODE(28, 11)
    LEAF_NODE(29, 15)
    LEAF_NODE(30, 14)
    0 /* We need something witout a comma at the end */
};

static int distance_tree[] = {
    BRANCH_NODE(0, 1, 2)
    BRANCH_NODE(1, 3, 4)
    BRANCH_NODE(2, 5, 6)
    BRANCH_NODE(3, 7, 8)
    BRANCH_NODE(4, 9, 10)
    BRANCH_NODE(5, 11, 12)
    LEAF_NODE(6, 0)
    BRANCH_NODE(7, 13, 14)
    BRANCH_NODE(8, 15, 16)
    BRANCH_NODE(9, 17, 18)
    BRANCH_NODE(10, 19, 20)
    BRANCH_NODE(11, 21, 22)
    BRANCH_NODE(12, 23, 24)
    BRANCH_NODE(13, 25, 26)
    BRANCH_NODE(14, 27, 28)
    BRANCH_NODE(15, 29, 30)
    BRANCH_NODE(16, 31, 32)
    BRANCH_NODE(17, 33, 34)
    BRANCH_NODE(18, 35, 36)
    BRANCH_NODE(19, 37, 38)
    BRANCH_NODE(20, 39, 40)
    BRANCH_NODE(21, 41, 42)
    BRANCH_NODE(22, 43, 44)
    LEAF_NODE(23, 2)
    LEAF_NODE(24, 1)
    BRANCH_NODE(25, 45, 46)
    BRANCH_NODE(26, 47, 48)
    BRANCH_NODE(27, 49, 50)
    BRANCH_NODE(28, 51, 52)
    BRANCH_NODE(29, 53, 54)
    BRANCH_NODE(30, 55, 56)
    BRANCH_NODE(31, 57, 58)
    BRANCH_NODE(32, 59, 60)
    BRANCH_NODE(33, 61, 62)
    BRANCH_NODE(34, 63, 64)
    BRANCH_NODE(35, 65, 66)
    BRANCH_NODE(36, 67, 68)
    BRANCH_NODE(37, 69, 70)
    BRANCH_NODE(38, 71, 72)
    BRANCH_NODE(39, 73, 74)
    BRANCH_NODE(40, 75, 76)
    LEAF_NODE(41, 6)
    LEAF_NODE(42, 5)
    LEAF_NODE(43, 4)
    LEAF_NODE(44, 3)
    BRANCH_NODE(45, 77, 78)
    BRANCH_NODE(46, 79, 80)
    BRANCH_NODE(47, 81, 82)
    BRANCH_NODE(48, 83, 84)
    BRANCH_NODE(49, 85, 86)
    BRANCH_NODE(50, 87, 88)
    BRANCH_NODE(51, 89, 90)
    BRANCH_NODE(52, 91, 92)
    BRANCH_NODE(53, 93, 94)
    BRANCH_NODE(54, 95, 96)
    BRANCH_NODE(55, 97, 98)
    BRANCH_NODE(56, 99, 100)
    BRANCH_NODE(57, 101, 102)
    BRANCH_NODE(58, 103, 104)
    BRANCH_NODE(59, 105, 106)
    BRANCH_NODE(60, 107, 108)
    BRANCH_NODE(61, 109, 110)
    LEAF_NODE(62, 21)
    LEAF_NODE(63, 20)
    LEAF_NODE(64, 19)
    LEAF_NODE(65, 18)
    LEAF_NODE(66, 17)
    LEAF_NODE(67, 16)
    LEAF_NODE(68, 15)
    LEAF_NODE(69, 14)
    LEAF_NODE(70, 13)
    LEAF_NODE(71, 12)
    LEAF_NODE(72, 11)
    LEAF_NODE(73, 10)
    LEAF_NODE(74, 9)
    LEAF_NODE(75, 8)
    LEAF_NODE(76, 7)
    BRANCH_NODE(77, 111, 112)
    BRANCH_NODE(78, 113, 114)
    BRANCH_NODE(79, 115, 116)
    BRANCH_NODE(80, 117, 118)
    BRANCH_NODE(81, 119, 120)
    BRANCH_NODE(82, 121, 122)
    BRANCH_NODE(83, 123, 124)
    BRANCH_NODE(84, 125, 126)
    LEAF_NODE(85, 47)
    LEAF_NODE(86, 46)
    LEAF_NODE(87, 45)
    LEAF_NODE(88, 44)
    LEAF_NODE(89, 43)
    LEAF_NODE(90, 42)
    LEAF_NODE(91, 41)
    LEAF_NODE(92, 40)
    LEAF_NODE(93, 39)
    LEAF_NODE(94, 38)
    LEAF_NODE(95, 37)
    LEAF_NODE(96, 36)
    LEAF_NODE(97, 35)
    LEAF_NODE(98, 34)
    LEAF_NODE(99, 33)
    LEAF_NODE(100, 32)
    LEAF_NODE(101, 31)
    LEAF_NODE(102, 30)
    LEAF_NODE(103, 29)
    LEAF_NODE(104, 28)
    LEAF_NODE(105, 27)
    LEAF_NODE(106, 26)
    LEAF_NODE(107, 25)
    LEAF_NODE(108, 24)
    LEAF_NODE(109, 23)
    LEAF_NODE(110, 22)
    LEAF_NODE(111, 63)
    LEAF_NODE(112, 62)
    LEAF_NODE(113, 61)
    LEAF_NODE(114, 60)
    LEAF_NODE(115, 59)
    LEAF_NODE(116, 58)
    LEAF_NODE(117, 57)
    LEAF_NODE(118, 56)
    LEAF_NODE(119, 55)
    LEAF_NODE(120, 54)
    LEAF_NODE(121, 53)
    LEAF_NODE(122, 52)
    LEAF_NODE(123, 51)
    LEAF_NODE(124, 50)
    LEAF_NODE(125, 49)
    LEAF_NODE(126, 48)
    0 /* We need something witout a comma at the end */
};

static int ascii_tree[] = {
    BRANCH_NODE(0, 1, 2)
    BRANCH_NODE(1, 3, 4)
    BRANCH_NODE(2, 5, 6)
    BRANCH_NODE(3, 7, 8)
    BRANCH_NODE(4, 9, 10)
    BRANCH_NODE(5, 11, 12)
    BRANCH_NODE(6, 13, 14)
    BRANCH_NODE(7, 15, 16)
    BRANCH_NODE(8, 17, 18)
    BRANCH_NODE(9, 19, 20)
    BRANCH_NODE(10, 21, 22)
    BRANCH_NODE(11, 23, 24)
    BRANCH_NODE(12, 25, 26)
    BRANCH_NODE(13, 27, 28)
    BRANCH_NODE(14, 29, 30)
    BRANCH_NODE(15, 31, 32)
    BRANCH_NODE(16, 33, 34)
    BRANCH_NODE(17, 35, 36)
    BRANCH_NODE(18, 37, 38)
    BRANCH_NODE(19, 39, 40)
    BRANCH_NODE(20, 41, 42)
    BRANCH_NODE(21, 43, 44)
    BRANCH_NODE(22, 45, 46)
    BRANCH_NODE(23, 47, 48)
    BRANCH_NODE(24, 49, 50)
    BRANCH_NODE(25, 51, 52)
    BRANCH_NODE(26, 53, 54)
    BRANCH_NODE(27, 55, 56)
    BRANCH_NODE(28, 57, 58)
    BRANCH_NODE(29, 59, 60)
    LEAF_NODE(30, 32)
    BRANCH_NODE(31, 61, 62)
    BRANCH_NODE(32, 63, 64)
    BRANCH_NODE(33, 65, 66)
    BRANCH_NODE(34, 67, 68)
    BRANCH_NODE(35, 69, 70)
    BRANCH_NODE(36, 71, 72)
    BRANCH_NODE(37, 73, 74)
    BRANCH_NODE(38, 75, 76)
    BRANCH_NODE(39, 77, 78)
    BRANCH_NODE(40, 79, 80)
    BRANCH_NODE(41, 81, 82)
    BRANCH_NODE(42, 83, 84)
    BRANCH_NODE(43, 85, 86)
    BRANCH_NODE(44, 87, 88)
    BRANCH_NODE(45, 89, 90)
    BRANCH_NODE(46, 91, 92)
    BRANCH_NODE(47, 93, 94)
    BRANCH_NODE(48, 95, 96)
    BRANCH_NODE(49, 97, 98)
    LEAF_NODE(50, 117)
    LEAF_NODE(51, 116)
    LEAF_NODE(52, 115)
    LEAF_NODE(53, 114)
    LEAF_NODE(54, 111)
    LEAF_NODE(55, 110)
    LEAF_NODE(56, 108)
    LEAF_NODE(57, 105)
    LEAF_NODE(58, 101)
    LEAF_NODE(59, 97)
    LEAF_NODE(60, 69)
    BRANCH_NODE(61, 99, 100)
    BRANCH_NODE(62, 101, 102)
    BRANCH_NODE(63, 103, 104)
    BRANCH_NODE(64, 105, 106)
    BRANCH_NODE(65, 107, 108)
    BRANCH_NODE(66, 109, 110)
    BRANCH_NODE(67, 111, 112)
    BRANCH_NODE(68, 113, 114)
    BRANCH_NODE(69, 115, 116)
    BRANCH_NODE(70, 117, 118)
    BRANCH_NODE(71, 119, 120)
    BRANCH_NODE(72, 121, 122)
    BRANCH_NODE(73, 123, 124)
    BRANCH_NODE(74, 125, 126)
    BRANCH_NODE(75, 127, 128)
    BRANCH_NODE(76, 129, 130)
    BRANCH_NODE(77, 131, 132)
    BRANCH_NODE(78, 133, 134)
    LEAF_NODE(79, 112)
    LEAF_NODE(80, 109)
    LEAF_NODE(81, 104)
    LEAF_NODE(82, 103)
    LEAF_NODE(83, 102)
    LEAF_NODE(84, 100)
    LEAF_NODE(85, 99)
    LEAF_NODE(86, 98)
    LEAF_NODE(87, 84)
    LEAF_NODE(88, 83)
    LEAF_NODE(89, 82)
    LEAF_NODE(90, 79)
    LEAF_NODE(91, 78)
    LEAF_NODE(92, 76)
    LEAF_NODE(93, 73)
    LEAF_NODE(94, 68)
    LEAF_NODE(95, 67)
    LEAF_NODE(96, 65)
    LEAF_NODE(97, 49)
    LEAF_NODE(98, 45)
    BRANCH_NODE(99, 135, 136)
    BRANCH_NODE(100, 137, 138)
    BRANCH_NODE(101, 139, 140)
    BRANCH_NODE(102, 141, 142)
    BRANCH_NODE(103, 143, 144)
    BRANCH_NODE(104, 145, 146)
    BRANCH_NODE(105, 147, 148)
    BRANCH_NODE(106, 149, 150)
    BRANCH_NODE(107, 151, 152)
    BRANCH_NODE(108, 153, 154)
    BRANCH_NODE(109, 155, 156)
    BRANCH_NODE(110, 157, 158)
    BRANCH_NODE(111, 159, 160)
    BRANCH_NODE(112, 161, 162)
    BRANCH_NODE(113, 163, 164)
    LEAF_NODE(114, 119)
    LEAF_NODE(115, 107)
    LEAF_NODE(116, 85)
    LEAF_NODE(117, 80)
    LEAF_NODE(118, 77)
    LEAF_NODE(119, 70)
    LEAF_NODE(120, 66)
    LEAF_NODE(121, 61)
    LEAF_NODE(122, 56)
    LEAF_NODE(123, 55)
    LEAF_NODE(124, 53)
    LEAF_NODE(125, 52)
    LEAF_NODE(126, 51)
    LEAF_NODE(127, 50)
    LEAF_NODE(128, 48)
    LEAF_NODE(129, 46)
    LEAF_NODE(130, 44)
    LEAF_NODE(131, 41)
    LEAF_NODE(132, 40)
    LEAF_NODE(133, 13)
    LEAF_NODE(134, 10)
    BRANCH_NODE(135, 165, 166)
    BRANCH_NODE(136, 167, 168)
    BRANCH_NODE(137, 169, 170)
    BRANCH_NODE(138, 171, 172)
    BRANCH_NODE(139, 173, 174)
    BRANCH_NODE(140, 175, 176)
    BRANCH_NODE(141, 177, 178)
    BRANCH_NODE(142, 179, 180)
    BRANCH_NODE(143, 181, 182)
    BRANCH_NODE(144, 183, 184)
    BRANCH_NODE(145, 185, 186)
    BRANCH_NODE(146, 187, 188)
    BRANCH_NODE(147, 189, 190)
    BRANCH_NODE(148, 191, 192)
    LEAF_NODE(149, 121)
    LEAF_NODE(150, 120)
    LEAF_NODE(151, 118)
    LEAF_NODE(152, 95)
    LEAF_NODE(153, 91)
    LEAF_NODE(154, 87)
    LEAF_NODE(155, 72)
    LEAF_NODE(156, 71)
    LEAF_NODE(157, 58)
    LEAF_NODE(158, 57)
    LEAF_NODE(159, 54)
    LEAF_NODE(160, 47)
    LEAF_NODE(161, 42)
    LEAF_NODE(162, 39)
    LEAF_NODE(163, 34)
    LEAF_NODE(164, 9)
    BRANCH_NODE(165, 193, 194)
    BRANCH_NODE(166, 195, 196)
    BRANCH_NODE(167, 197, 198)
    BRANCH_NODE(168, 199, 200)
    BRANCH_NODE(169, 201, 202)
    BRANCH_NODE(170, 203, 204)
    BRANCH_NODE(171, 205, 206)
    BRANCH_NODE(172, 207, 208)
    BRANCH_NODE(173, 209, 210)
    BRANCH_NODE(174, 211, 212)
    BRANCH_NODE(175, 213, 214)
    BRANCH_NODE(176, 215, 216)
    BRANCH_NODE(177, 217, 218)
    BRANCH_NODE(178, 219, 220)
    BRANCH_NODE(179, 221, 222)
    BRANCH_NODE(180, 223, 224)
    BRANCH_NODE(181, 225, 226)
    BRANCH_NODE(182, 227, 228)
    BRANCH_NODE(183, 229, 230)
    BRANCH_NODE(184, 231, 232)
    BRANCH_NODE(185, 233, 234)
    LEAF_NODE(186, 93)
    LEAF_NODE(187, 89)
    LEAF_NODE(188, 88)
    LEAF_NODE(189, 86)
    LEAF_NODE(190, 75)
    LEAF_NODE(191, 62)
    LEAF_NODE(192, 43)
    BRANCH_NODE(193, 235, 236)
    BRANCH_NODE(194, 237, 238)
    BRANCH_NODE(195, 239, 240)
    BRANCH_NODE(196, 241, 242)
    BRANCH_NODE(197, 243, 244)
    BRANCH_NODE(198, 245, 246)
    BRANCH_NODE(199, 247, 248)
    BRANCH_NODE(200, 249, 250)
    BRANCH_NODE(201, 251, 252)
    BRANCH_NODE(202, 253, 254)
    BRANCH_NODE(203, 255, 256)
    BRANCH_NODE(204, 257, 258)
    BRANCH_NODE(205, 259, 260)
    BRANCH_NODE(206, 261, 262)
    BRANCH_NODE(207, 263, 264)
    BRANCH_NODE(208, 265, 266)
    BRANCH_NODE(209, 267, 268)
    BRANCH_NODE(210, 269, 270)
    BRANCH_NODE(211, 271, 272)
    BRANCH_NODE(212, 273, 274)
    BRANCH_NODE(213, 275, 276)
    BRANCH_NODE(214, 277, 278)
    BRANCH_NODE(215, 279, 280)
    BRANCH_NODE(216, 281, 282)
    BRANCH_NODE(217, 283, 284)
    BRANCH_NODE(218, 285, 286)
    BRANCH_NODE(219, 287, 288)
    BRANCH_NODE(220, 289, 290)
    BRANCH_NODE(221, 291, 292)
    BRANCH_NODE(222, 293, 294)
    BRANCH_NODE(223, 295, 296)
    BRANCH_NODE(224, 297, 298)
    BRANCH_NODE(225, 299, 300)
    BRANCH_NODE(226, 301, 302)
    BRANCH_NODE(227, 303, 304)
    BRANCH_NODE(228, 305, 306)
    BRANCH_NODE(229, 307, 308)
    LEAF_NODE(230, 122)
    LEAF_NODE(231, 113)
    LEAF_NODE(232, 38)
    LEAF_NODE(233, 36)
    LEAF_NODE(234, 33)
    BRANCH_NODE(235, 309, 310)
    BRANCH_NODE(236, 311, 312)
    BRANCH_NODE(237, 313, 314)
    BRANCH_NODE(238, 315, 316)
    BRANCH_NODE(239, 317, 318)
    BRANCH_NODE(240, 319, 320)
    BRANCH_NODE(241, 321, 322)
    BRANCH_NODE(242, 323, 324)
    BRANCH_NODE(243, 325, 326)
    BRANCH_NODE(244, 327, 328)
    BRANCH_NODE(245, 329, 330)
    BRANCH_NODE(246, 331, 332)
    BRANCH_NODE(247, 333, 334)
    BRANCH_NODE(248, 335, 336)
    BRANCH_NODE(249, 337, 338)
    BRANCH_NODE(250, 339, 340)
    BRANCH_NODE(251, 341, 342)
    BRANCH_NODE(252, 343, 344)
    BRANCH_NODE(253, 345, 346)
    BRANCH_NODE(254, 347, 348)
    BRANCH_NODE(255, 349, 350)
    BRANCH_NODE(256, 351, 352)
    BRANCH_NODE(257, 353, 354)
    BRANCH_NODE(258, 355, 356)
    BRANCH_NODE(259, 357, 358)
    BRANCH_NODE(260, 359, 360)
    BRANCH_NODE(261, 361, 362)
    BRANCH_NODE(262, 363, 364)
    BRANCH_NODE(263, 365, 366)
    BRANCH_NODE(264, 367, 368)
    BRANCH_NODE(265, 369, 370)
    BRANCH_NODE(266, 371, 372)
    BRANCH_NODE(267, 373, 374)
    BRANCH_NODE(268, 375, 376)
    BRANCH_NODE(269, 377, 378)
    BRANCH_NODE(270, 379, 380)
    BRANCH_NODE(271, 381, 382)
    BRANCH_NODE(272, 383, 384)
    BRANCH_NODE(273, 385, 386)
    BRANCH_NODE(274, 387, 388)
    BRANCH_NODE(275, 389, 390)
    BRANCH_NODE(276, 391, 392)
    BRANCH_NODE(277, 393, 394)
    BRANCH_NODE(278, 395, 396)
    BRANCH_NODE(279, 397, 398)
    BRANCH_NODE(280, 399, 400)
    BRANCH_NODE(281, 401, 402)
    BRANCH_NODE(282, 403, 404)
    BRANCH_NODE(283, 405, 406)
    BRANCH_NODE(284, 407, 408)
    BRANCH_NODE(285, 409, 410)
    BRANCH_NODE(286, 411, 412)
    BRANCH_NODE(287, 413, 414)
    BRANCH_NODE(288, 415, 416)
    BRANCH_NODE(289, 417, 418)
    BRANCH_NODE(290, 419, 420)
    BRANCH_NODE(291, 421, 422)
    BRANCH_NODE(292, 423, 424)
    BRANCH_NODE(293, 425, 426)
    BRANCH_NODE(294, 427, 428)
    BRANCH_NODE(295, 429, 430)
    BRANCH_NODE(296, 431, 432)
    BRANCH_NODE(297, 433, 434)
    BRANCH_NODE(298, 435, 436)
    LEAF_NODE(299, 124)
    LEAF_NODE(300, 123)
    LEAF_NODE(301, 106)
    LEAF_NODE(302, 92)
    LEAF_NODE(303, 90)
    LEAF_NODE(304, 81)
    LEAF_NODE(305, 74)
    LEAF_NODE(306, 63)
    LEAF_NODE(307, 60)
    LEAF_NODE(308, 0)
    BRANCH_NODE(309, 437, 438)
    BRANCH_NODE(310, 439, 440)
    BRANCH_NODE(311, 441, 442)
    BRANCH_NODE(312, 443, 444)
    BRANCH_NODE(313, 445, 446)
    BRANCH_NODE(314, 447, 448)
    BRANCH_NODE(315, 449, 450)
    BRANCH_NODE(316, 451, 452)
    BRANCH_NODE(317, 453, 454)
    BRANCH_NODE(318, 455, 456)
    BRANCH_NODE(319, 457, 458)
    BRANCH_NODE(320, 459, 460)
    BRANCH_NODE(321, 461, 462)
    BRANCH_NODE(322, 463, 464)
    BRANCH_NODE(323, 465, 466)
    BRANCH_NODE(324, 467, 468)
    BRANCH_NODE(325, 469, 470)
    BRANCH_NODE(326, 471, 472)
    BRANCH_NODE(327, 473, 474)
    BRANCH_NODE(328, 475, 476)
    BRANCH_NODE(329, 477, 478)
    BRANCH_NODE(330, 479, 480)
    BRANCH_NODE(331, 481, 482)
    BRANCH_NODE(332, 483, 484)
    BRANCH_NODE(333, 485, 486)
    BRANCH_NODE(334, 487, 488)
    BRANCH_NODE(335, 489, 490)
    BRANCH_NODE(336, 491, 492)
    BRANCH_NODE(337, 493, 494)
    BRANCH_NODE(338, 495, 496)
    BRANCH_NODE(339, 497, 498)
    BRANCH_NODE(340, 499, 500)
    BRANCH_NODE(341, 501, 502)
    BRANCH_NODE(342, 503, 504)
    BRANCH_NODE(343, 505, 506)
    BRANCH_NODE(344, 507, 508)
    BRANCH_NODE(345, 509, 510)
    LEAF_NODE(346, 244)
    LEAF_NODE(347, 243)
    LEAF_NODE(348, 242)
    LEAF_NODE(349, 238)
    LEAF_NODE(350, 233)
    LEAF_NODE(351, 229)
    LEAF_NODE(352, 225)
    LEAF_NODE(353, 223)
    LEAF_NODE(354, 222)
    LEAF_NODE(355, 221)
    LEAF_NODE(356, 220)
    LEAF_NODE(357, 219)
    LEAF_NODE(358, 218)
    LEAF_NODE(359, 217)
    LEAF_NODE(360, 216)
    LEAF_NODE(361, 215)
    LEAF_NODE(362, 214)
    LEAF_NODE(363, 213)
    LEAF_NODE(364, 212)
    LEAF_NODE(365, 211)
    LEAF_NODE(366, 210)
    LEAF_NODE(367, 209)
    LEAF_NODE(368, 208)
    LEAF_NODE(369, 207)
    LEAF_NODE(370, 206)
    LEAF_NODE(371, 205)
    LEAF_NODE(372, 204)
    LEAF_NODE(373, 203)
    LEAF_NODE(374, 202)
    LEAF_NODE(375, 201)
    LEAF_NODE(376, 200)
    LEAF_NODE(377, 199)
    LEAF_NODE(378, 198)
    LEAF_NODE(379, 197)
    LEAF_NODE(380, 196)
    LEAF_NODE(381, 195)
    LEAF_NODE(382, 194)
    LEAF_NODE(383, 193)
    LEAF_NODE(384, 192)
    LEAF_NODE(385, 191)
    LEAF_NODE(386, 190)
    LEAF_NODE(387, 189)
    LEAF_NODE(388, 188)
    LEAF_NODE(389, 187)
    LEAF_NODE(390, 186)
    LEAF_NODE(391, 185)
    LEAF_NODE(392, 184)
    LEAF_NODE(393, 183)
    LEAF_NODE(394, 182)
    LEAF_NODE(395, 181)
    LEAF_NODE(396, 180)
    LEAF_NODE(397, 179)
    LEAF_NODE(398, 178)
    LEAF_NODE(399, 177)
    LEAF_NODE(400, 176)
    LEAF_NODE(401, 127)
    LEAF_NODE(402, 126)
    LEAF_NODE(403, 125)
    LEAF_NODE(404, 96)
    LEAF_NODE(405, 94)
    LEAF_NODE(406, 64)
    LEAF_NODE(407, 59)
    LEAF_NODE(408, 37)
    LEAF_NODE(409, 35)
    LEAF_NODE(410, 31)
    LEAF_NODE(411, 30)
    LEAF_NODE(412, 29)
    LEAF_NODE(413, 28)
    LEAF_NODE(414, 27)
    LEAF_NODE(415, 25)
    LEAF_NODE(416, 24)
    LEAF_NODE(417, 23)
    LEAF_NODE(418, 22)
    LEAF_NODE(419, 21)
    LEAF_NODE(420, 20)
    LEAF_NODE(421, 19)
    LEAF_NODE(422, 18)
    LEAF_NODE(423, 17)
    LEAF_NODE(424, 16)
    LEAF_NODE(425, 15)
    LEAF_NODE(426, 14)
    LEAF_NODE(427, 12)
    LEAF_NODE(428, 11)
    LEAF_NODE(429, 8)
    LEAF_NODE(430, 7)
    LEAF_NODE(431, 6)
    LEAF_NODE(432, 5)
    LEAF_NODE(433, 4)
    LEAF_NODE(434, 3)
    LEAF_NODE(435, 2)
    LEAF_NODE(436, 1)
    LEAF_NODE(437, 255)
    LEAF_NODE(438, 254)
    LEAF_NODE(439, 253)
    LEAF_NODE(440, 252)
    LEAF_NODE(441, 251)
    LEAF_NODE(442, 250)
    LEAF_NODE(443, 249)
    LEAF_NODE(444, 248)
    LEAF_NODE(445, 247)
    LEAF_NODE(446, 246)
    LEAF_NODE(447, 245)
    LEAF_NODE(448, 241)
    LEAF_NODE(449, 240)
    LEAF_NODE(450, 239)
    LEAF_NODE(451, 237)
    LEAF_NODE(452, 236)
    LEAF_NODE(453, 235)
    LEAF_NODE(454, 234)
    LEAF_NODE(455, 232)
    LEAF_NODE(456, 231)
    LEAF_NODE(457, 230)
    LEAF_NODE(458, 228)
    LEAF_NODE(459, 227)
    LEAF_NODE(460, 226)
    LEAF_NODE(461, 224)
    LEAF_NODE(462, 175)
    LEAF_NODE(463, 174)
    LEAF_NODE(464, 173)
    LEAF_NODE(465, 172)
    LEAF_NODE(466, 171)
    LEAF_NODE(467, 170)
    LEAF_NODE(468, 169)
    LEAF_NODE(469, 168)
    LEAF_NODE(470, 167)
    LEAF_NODE(471, 166)
    LEAF_NODE(472, 165)
    LEAF_NODE(473, 164)
    LEAF_NODE(474, 163)
    LEAF_NODE(475, 162)
    LEAF_NODE(476, 161)
    LEAF_NODE(477, 160)
    LEAF_NODE(478, 159)
    LEAF_NODE(479, 158)
    LEAF_NODE(480, 157)
    LEAF_NODE(481, 156)
    LEAF_NODE(482, 155)
    LEAF_NODE(483, 154)
    LEAF_NODE(484, 153)
    LEAF_NODE(485, 152)
    LEAF_NODE(486, 151)
    LEAF_NODE(487, 150)
    LEAF_NODE(488, 149)
    LEAF_NODE(489, 148)
    LEAF_NODE(490, 147)
    LEAF_NODE(491, 146)
    LEAF_NODE(492, 145)
    LEAF_NODE(493, 144)
    LEAF_NODE(494, 143)
    LEAF_NODE(495, 142)
    LEAF_NODE(496, 141)
    LEAF_NODE(497, 140)
    LEAF_NODE(498, 139)
    LEAF_NODE(499, 138)
    LEAF_NODE(500, 137)
    LEAF_NODE(501, 136)
    LEAF_NODE(502, 135)
    LEAF_NODE(503, 134)
    LEAF_NODE(504, 133)
    LEAF_NODE(505, 132)
    LEAF_NODE(506, 131)
    LEAF_NODE(507, 130)
    LEAF_NODE(508, 129)
    LEAF_NODE(509, 128)
    LEAF_NODE(510, 26)
    0 /* We need something witout a comma at the end */
};

#define CALLC(x) { if ((x) == -SCI_ERROR_DECOMPRESSION_OVERFLOW) return -SCI_ERROR_DECOMPRESSION_OVERFLOW; }

static inline int
getbits_msb_first(struct bit_read_struct *inp, int bits)
{
    int morebytes = (bits + inp->bitpos - 1) >> 3;
    int result = 0;
    int i;

    if (inp->bytepos + morebytes >= inp->length) {
        fprintf(stderr, "read out-of-bounds with bytepos %d + morebytes %d >= length %d\n",
            inp->bytepos, morebytes, inp->length);
        return -SCI_ERROR_DECOMPRESSION_OVERFLOW;
    }

    for (i = 0; i <= morebytes; i++)
        result |= (inp->data[inp->bytepos + i]) << (i << 3);

    result >>= inp->bitpos;
    result &= ~(~0 << bits);

    inp->bitpos += bits - (morebytes << 3);
    inp->bytepos += morebytes;

    return result;
}

static int DEBUG_DCL_INFLATE = 0; /* FIXME: Make this a define eventually */

static inline int
getbits(struct bit_read_struct *inp, int bits)
{
    int morebytes = (bits + inp->bitpos - 1) >> 3;
    int result = 0;
    int i;

    if (inp->bytepos + morebytes >= inp->length) {
        fprintf(stderr, "read out-of-bounds with bytepos %d + morebytes %d >= length %d\n",
            inp->bytepos, morebytes, inp->length);
        return -SCI_ERROR_DECOMPRESSION_OVERFLOW;
    }

    for (i = 0; i <= morebytes; i++)
        result |= (inp->data[inp->bytepos + i]) << (i << 3);

    result >>= inp->bitpos;
    result &= ~((~0) << bits);

    inp->bitpos += bits - (morebytes << 3);
    inp->bytepos += morebytes;

    if (DEBUG_DCL_INFLATE)
        fprintf(stderr, "(%d:%04x)", bits, result);

    return result;
}

static int
huffman_lookup(struct bit_read_struct *inp, int *tree)
{
    int pos = 0;
    int bit;

    while (!(tree[pos] & HUFFMAN_LEAF)) {
        CALLC(bit = getbits(inp, 1));
        if (DEBUG_DCL_INFLATE)
            fprintf(stderr, "[%d]:%d->", pos, bit);
        if (bit)
            pos = tree[pos] & ~(~0 << BRANCH_SHIFT);
        else
            pos = tree[pos] >> BRANCH_SHIFT;
    }
    if (DEBUG_DCL_INFLATE)
        fprintf(stderr, "=%02x\n", tree[pos] & 0xffff);
    return tree[pos] & 0xffff;
}

#define VALUE_M(i) ((i == 0)? 7 : (VALUE_M(i - 1) + 2**i));

#define DCL_ASCII_MODE 1

static int
decrypt4_hdyn(byte *dest, int length, struct bit_read_struct *reader)
{
    int mode, length_param, value, val_length, val_distance;
    int write_pos = 0;
    int M[] = { 0x07, 0x08, 0x0A, 0x0E, 0x16, 0x26, 0x46, 0x86, 0x106 };

    CALLC(mode = getbits(reader, 8));
    CALLC(length_param = getbits(reader, 8));

    if (mode == DCL_ASCII_MODE) {
        fprintf(stderr, "DCL-INFLATE: Warning: Decompressing ASCII mode (untested)\n");
        /*		DEBUG_DCL_INFLATE = 1; */
    }
    else if (mode) {
        fprintf(stderr, "DCL-INFLATE: Error: Encountered mode %02x, expected 00 or 01\n", mode);
        return 1;
    }

    if (DEBUG_DCL_INFLATE) {
        int i;
        for (i = 0; i < reader->length; i++) {
            fprintf(stderr, "%02x ", reader->data[i]);
            if (!((i + 1) & 0x1f))
                fprintf(stderr, "\n");
        }


        fprintf(stderr, "\n---\n");
    }


    if (length_param < 3 || length_param > 6)
        fprintf(stderr, "Warning: Unexpected length_param value %d (expected in [3,6])\n", length_param);

    while (write_pos < length) {
        CALLC(value = getbits(reader, 1));

        if (value) { /* (length,distance) pair */
            CALLC(value = huffman_lookup(reader, length_tree));

            if (value < 8)
                val_length = value + 2;
            else {
                int length_bonus;

                val_length = M[value - 7] + 2;
                CALLC(length_bonus = getbits(reader, value - 7));
                val_length += length_bonus;
            }

            if (DEBUG_DCL_INFLATE)
                fprintf(stderr, " | ");

            CALLC(value = huffman_lookup(reader, distance_tree));

            if (val_length == 2) {
                val_distance = value << 2;

                CALLC(value = getbits(reader, 2));
                val_distance |= value;
            }
            else {
                val_distance = value << length_param;

                CALLC(value = getbits(reader, length_param));
                val_distance |= value;
            }
            ++val_distance;

            if (DEBUG_DCL_INFLATE)
                fprintf(stderr, "\nCOPY(%d from %d)\n", val_length, val_distance);

            if (val_length + write_pos > length) {
                fprintf(stderr, "DCL-INFLATE Error: Write out of bounds while copying %d bytes\n", val_length);
                return -SCI_ERROR_DECOMPRESSION_OVERFLOW;
            }

            if (write_pos < val_distance) {
                fprintf(stderr, "DCL-INFLATE Error: Attempt to copy from before beginning of input stream\n");
                return -SCI_ERROR_DECOMPRESSION_INSANE;
            }

            while (val_length) {
                int copy_length = (val_length > val_distance) ? val_distance : val_length;

                memcpy(dest + write_pos, dest + write_pos - val_distance, copy_length);

                if (DEBUG_DCL_INFLATE) {
                    int i;
                    for (i = 0; i < copy_length; i++)
                        fprintf(stderr, "\33[32;31m%02x\33[37;37m ", dest[write_pos + i]);
                    fprintf(stderr, "\n");
                }

                val_length -= copy_length;
                val_distance += copy_length;
                write_pos += copy_length;
            }

        }
        else { /* Copy byte verbatim */
            if (mode == DCL_ASCII_MODE) {
                CALLC(value = huffman_lookup(reader, ascii_tree));
            }
            else {
                CALLC(value = getbits(reader, 8));
            }

            dest[write_pos++] = value;

            if (DEBUG_DCL_INFLATE)
                fprintf(stderr, "\33[32;31m%02x \33[37;37m", value);
        }
    }

    return 0;
}

int decrypt4(byte* dest, byte* src, int length, int complength)
{
    struct bit_read_struct reader;

    reader.length = complength;
    reader.bitpos = 0;
    reader.bytepos = 0;
    reader.data = src;

    return -decrypt4_hdyn(dest, length, &reader);
}


