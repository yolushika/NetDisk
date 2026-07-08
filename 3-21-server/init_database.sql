-- ============================================
-- NetDisk 数据库初始化脚本
-- 导出自 MySQL 5.7.17，数据库名: 3-22NetDisk
-- ============================================

CREATE DATABASE IF NOT EXISTS `3-22NetDisk`
    DEFAULT CHARACTER SET utf8
    COLLATE utf8_general_ci;

USE `3-22NetDisk`;

-- ============================================
-- 用户表
-- ============================================
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
    `u_id`       BIGINT(11)  NOT NULL AUTO_INCREMENT COMMENT '用户ID',
    `u_name`     VARCHAR(45) NOT NULL             COMMENT '用户名',
    `u_password` VARCHAR(45) NOT NULL             COMMENT '密码',
    `u_tel`      BIGINT(11)  NOT NULL             COMMENT '手机号',
    PRIMARY KEY (`u_id`),
    UNIQUE KEY `u_name_UNIQUE` (`u_name`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8 COMMENT='用户表';

-- ============================================
-- 文件表（按 MD5 去重，引用计数管理）
-- ============================================
DROP TABLE IF EXISTS `file`;
CREATE TABLE `file` (
    `f_id`         BIGINT(11)   NOT NULL AUTO_INCREMENT COMMENT '文件ID',
    `f_name`       VARCHAR(45)  NOT NULL                COMMENT '文件名',
    `f_size`       BIGINT(11)   DEFAULT '0'             COMMENT '文件大小（字节）',
    `f_uploadtime` DATETIME     DEFAULT CURRENT_TIMESTAMP COMMENT '首次上传时间',
    `f_path`       VARCHAR(260) NOT NULL                COMMENT '服务器存储路径',
    `f_count`      BIGINT(11)   DEFAULT '1'             COMMENT '引用计数',
    `f_md5`        VARCHAR(45)  DEFAULT 'NULL'          COMMENT '文件MD5（秒传判断）',
    PRIMARY KEY (`f_id`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=utf8 COMMENT='文件表';

-- ============================================
-- 用户-文件映射表
-- ============================================
DROP TABLE IF EXISTS `user_file`;
CREATE TABLE `user_file` (
    `num`  BIGINT(11) NOT NULL AUTO_INCREMENT COMMENT '记录编号',
    `u_id` BIGINT(11) NOT NULL                 COMMENT '用户ID',
    `f_id` BIGINT(11) NOT NULL                 COMMENT '文件ID',
    `time` DATETIME   NOT NULL                 COMMENT '上传时间',
    PRIMARY KEY (`f_id`, `u_id`),
    UNIQUE KEY `num_UNIQUE` (`num`)
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=utf8 COMMENT='用户文件映射表';

-- ============================================
-- 分享表
-- ============================================
DROP TABLE IF EXISTS `user_shared`;
CREATE TABLE `user_shared` (
    `number` INT(11)     NOT NULL AUTO_INCREMENT COMMENT '记录编号',
    `uid`    BIGINT(11)  NOT NULL                 COMMENT '分享者用户ID',
    `fid`    BIGINT(11)  NOT NULL                 COMMENT '被分享文件ID',
    `code`   VARCHAR(10) NOT NULL                 COMMENT '分享提取码',
    PRIMARY KEY (`fid`, `uid`),
    UNIQUE KEY `number_UNIQUE` (`number`),
    UNIQUE KEY `code_UNIQUE` (`code`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8 COMMENT='文件分享表';

-- ============================================
-- 用户文件视图（user_file LEFT JOIN file）
-- ============================================
CREATE OR REPLACE
    ALGORITHM = UNDEFINED
    DEFINER = `root`@`localhost`
    SQL SECURITY DEFINER
VIEW `ufile` AS
    SELECT
        uf.u_id,
        f.f_id,
        f.f_name,
        f.f_size,
        uf.time   AS f_uploadtime,
        f.f_path,
        f.f_count,
        f.f_md5
    FROM user_file uf
    LEFT JOIN file f ON uf.f_id = f.f_id;
