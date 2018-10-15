SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for `custmeta`
-- ----------------------------
DROP TABLE IF EXISTS `custmeta`;
CREATE TABLE `custmeta` (
  `_id` binary(32) NOT NULL,
  `url` varchar(1024) CHARACTER SET utf8 COLLATE utf8_bin NOT NULL,
  `status` int(11) NOT NULL,
  `1` double NOT NULL DEFAULT '-1e10' COMMENT '地理范围MaxX',
  `2` double NOT NULL DEFAULT '-1e10' COMMENT '地理范围MinX',
  `3` double NOT NULL DEFAULT '-1e10' COMMENT '地理范围MaxY',
  `4` double NOT NULL DEFAULT '-1e10' COMMENT '地理范围MinY',
  `5` geometry NOT NULL COMMENT '地理范围((西南，东南，东北，西北))',
  `6` varchar(128) NOT NULL DEFAULT '[-1 -1,-1 -1,-1 -1,-1 -1]' COMMENT '地理范围角点',
  `7` varchar(256) NOT NULL DEFAULT '' COMMENT '元数据文件名称',
  `8` varchar(256) NOT NULL DEFAULT '' COMMENT '影像类型',
  `9` bigint NULL DEFAULT '-2147483647' COMMENT '产品生产时间',
  `10` double NOT NULL DEFAULT '-1e10' COMMENT '分辨率',
  `11` varchar(64) NOT NULL DEFAULT '' COMMENT '数据格式',
  `12` varchar(64) NOT NULL DEFAULT '' COMMENT '椭球长半径',
  `13` varchar(64) NOT NULL DEFAULT '' COMMENT '椭球扁率',
  `14` varchar(64) NOT NULL DEFAULT '' COMMENT '所采用大地基准',
  `15` varchar(256) NOT NULL DEFAULT '' COMMENT '地图投影',
  `16` bigint NULL DEFAULT '-2147483647' COMMENT '中央子午线',
  `17` varchar(32) NOT NULL DEFAULT '' COMMENT '分带方式',
  `18` bigint NULL DEFAULT '-2147483647' COMMENT '高斯-克吕格投影带号',
  `19` varchar(32) NOT NULL DEFAULT '' COMMENT '坐标单位',
  `20` varchar(32) NOT NULL DEFAULT '' COMMENT '高程系统名',
  `21` varchar(64) NOT NULL DEFAULT '' COMMENT '高程基准',
  PRIMARY KEY (`_id`),
  UNIQUE KEY `index_url` (`url`),
  SPATIAL KEY `box` (`5`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DELIMITER ;
DROP TRIGGER IF EXISTS `insert_custmeta`;
DELIMITER ;;
CREATE TRIGGER `insert_custmeta` AFTER INSERT ON `image` FOR EACH ROW begin 
	insert into custmeta(_id, url, status,`5`) values(new._id, new.url, 1,ST_GeomFromText('POLYGON((-1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10, -1e10 -1e10))'));
end
;;
DELIMITER ;
DROP TRIGGER IF EXISTS `delete_custmeta`;
DELIMITER ;;
CREATE TRIGGER `delete_custmeta` AFTER DELETE ON `image` FOR EACH ROW begin 
	delete from custmeta where _id = old._id; 
end
;;
DELIMITER ;