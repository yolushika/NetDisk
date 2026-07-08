-- ============================================
-- NetDisk 数据库初始化脚本
-- 数据库名: 3-22NetDisk
-- ============================================

-- 创建数据库
CREATE DATABASE IF NOT EXISTS `3-22NetDisk` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;
USE `3-22NetDisk`;

-- ============================================
-- 用户表
-- ============================================
CREATE TABLE IF NOT EXISTS `user` (
    `u_id`       INT AUTO_INCREMENT PRIMARY KEY COMMENT '用户ID',
    `u_name`     VARCHAR(45)  NOT NULL UNIQUE    COMMENT '用户名',
    `u_password` VARCHAR(45)  NOT NULL            COMMENT '密码',
    `u_tel`      BIGINT       NOT NULL            COMMENT '手机号'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户表';

-- ============================================
-- 文件表（物理文件元信息，按 MD5 去重）
-- ============================================
CREATE TABLE IF NOT EXISTS `file` (
    `f_id`         INT AUTO_INCREMENT PRIMARY KEY COMMENT '文件ID',
    `f_name`       VARCHAR(260) NOT NULL          COMMENT '文件名',
    `f_uploadtime` VARCHAR(45)  NOT NULL          COMMENT '首次上传时间',
    `f_size`       BIGINT       NOT NULL DEFAULT 0 COMMENT '文件大小（字节）',
    `f_MD5`        VARCHAR(45)  NOT NULL          COMMENT '文件MD5（用于秒传判断）',
    `f_path`       VARCHAR(260) NOT NULL          COMMENT '服务器存储路径',
    `f_count`      INT          NOT NULL DEFAULT 1 COMMENT '引用计数（多个用户持有同一文件时递增）'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='文件表';

-- ============================================
-- 用户-文件映射表
-- ============================================
CREATE TABLE IF NOT EXISTS `user_file` (
    `id`   INT AUTO_INCREMENT PRIMARY KEY COMMENT '记录ID',
    `u_id` BIGINT       NOT NULL                 COMMENT '用户ID',
    `f_id` BIGINT       NOT NULL                 COMMENT '文件ID',
    `time` VARCHAR(45)  NOT NULL                 COMMENT '上传时间',
    INDEX `idx_uid` (`u_id`),
    INDEX `idx_fid` (`f_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='用户文件映射表';

-- ============================================
-- 用户文件视图（方便查询用户文件列表）
-- ============================================
CREATE OR REPLACE VIEW `ufile` AS
SELECT
    uf.u_id,
    uf.f_id,
    f.f_name,
    f.f_size,
    f.f_count,
    f.f_path,
    uf.time
FROM user_file uf
JOIN file f ON uf.f_id = f.f_id;

-- ============================================
-- 分享表
-- ============================================
CREATE TABLE IF NOT EXISTS `user_shared` (
    `id`   INT AUTO_INCREMENT PRIMARY KEY COMMENT '记录ID',
    `uid`  BIGINT       NOT NULL                 COMMENT '分享者用户ID',
    `fid`  BIGINT       NOT NULL                 COMMENT '被分享的文件ID',
    `code` VARCHAR(45)  NOT NULL                 COMMENT '分享提取码',
    INDEX `idx_code` (`code`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='文件分享表';
