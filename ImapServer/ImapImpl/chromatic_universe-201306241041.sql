-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (i686)
--
-- Host: localhost    Database: chromatic_universe
-- ------------------------------------------------------
-- Server version	5.5.31-0ubuntu0.12.10.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `chromaticarchivehash`
--

DROP TABLE IF EXISTS `chromaticarchivehash`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticarchivehash` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `absolute_url_string` varchar(255) NOT NULL,
  `relative_url_string` varchar(255) NOT NULL DEFAULT '',
  `mbx` varchar(255) NOT NULL DEFAULT '',
  `has_attachments` int(11) NOT NULL DEFAULT '0',
  `msg_size` int(11) NOT NULL DEFAULT '0',
  `delivery_date` varchar(100) NOT NULL DEFAULT '',
  `archived` int(1) NOT NULL DEFAULT '0',
  `pending` int(1) DEFAULT '0',
  PRIMARY KEY (`idx`),
  UNIQUE KEY `absolute_url_string` (`absolute_url_string`),
  UNIQUE KEY `idx` (`idx`),
  KEY `Mailbox` (`mbx`)
) ENGINE=MyISAM AUTO_INCREMENT=243 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticarchivehash`
--

LOCK TABLES `chromaticarchivehash` WRITE;
/*!40000 ALTER TABLE `chromaticarchivehash` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticarchivehash` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER chromaticarchivehash_before_ins_tr BEFORE INSERT ON chromaticarchivehash   FOR EACH ROW BEGIN     DECLARE strFolder VARCHAR( 255 );     DECLARE strSubFolder VARCHAR( 255 );               SELECT NEW.mbx INTO strFolder;          SELECT SUBSTRING( strFolder , INSTR( strFolder , 'MBX' ) + LENGTH( 'MBX' ) )     INTO strSubFolder;          SELECT SUBSTRING( strSubFolder , INSTR( strSubFolder , '/' ) + LENGTH( '/' ) - 1 )     INTO strSubFolder;                     UPDATE chromaticsubscribefolders     SET marked = 1     WHERE folder_name = strSubFolder;             INSERT INTO chromaticmessageflags( absolute_url_string ,     				              answered ,     					          deleted ,     							  draft ,     					          flagged ,     						      recent ,     			                  seen )            						  VALUES( NEW.absolute_url_string ,                                           0 ,                                           0 ,                                           0 ,                                           0 ,                                           1 ,                                           0 );                                                      END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `chromaticarchivehash_after_upd_tr` AFTER UPDATE ON `chromaticarchivehash`   FOR EACH ROW BEGIN    IF ( OLD.archived = 0 ) AND ( NEW.archived = 1 ) THEN      INSERT INTO chromaticmessagefolderoffset ( absolute_url_string ,                                           folder_offset )                                           VALUES( NEW.absolute_url_string ,                                           DATE_FORMAT( NOW() , '%m%d%y') );    END IF;  END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER chromaticarchivehash_before_del_tr AFTER DELETE ON chromaticarchivehash   FOR EACH ROW BEGIN     DELETE FROM chromaticmessagefolderoffset     WHERE chromaticmessagefolderoffset.absolute_url_string = OLD.absolute_url_string;          DELETE FROM chromaticmessageflags     WHERE chromaticmessageflags.absolute_url_string = OLD.absolute_url_string; END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Table structure for table `chromaticdomainrepository`
--

DROP TABLE IF EXISTS `chromaticdomainrepository`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticdomainrepository` (
  `domain_name` varchar(255) NOT NULL DEFAULT '',
  `mail_server` varchar(255) DEFAULT NULL,
  UNIQUE KEY `domain_name` (`domain_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticdomainrepository`
--

LOCK TABLES `chromaticdomainrepository` WRITE;
/*!40000 ALTER TABLE `chromaticdomainrepository` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticdomainrepository` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticflashmessage`
--

DROP TABLE IF EXISTS `chromaticflashmessage`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticflashmessage` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `moniker` varchar(100) NOT NULL DEFAULT '',
  `permanent_url` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`,`moniker`)
) ENGINE=MyISAM AUTO_INCREMENT=986 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticflashmessage`
--

LOCK TABLES `chromaticflashmessage` WRITE;
/*!40000 ALTER TABLE `chromaticflashmessage` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticflashmessage` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticfoldersubscriptions`
--

DROP TABLE IF EXISTS `chromaticfoldersubscriptions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticfoldersubscriptions` (
  `subscription_idx` int(11) NOT NULL AUTO_INCREMENT,
  `idx` int(11) NOT NULL,
  `mail_nickname` varchar(255) NOT NULL,
  PRIMARY KEY (`subscription_idx`),
  UNIQUE KEY `idx_subscribe` (`idx`,`mail_nickname`),
  KEY `idx` (`idx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=16384 COMMENT='InnoDB free: 4388864 kB';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticfoldersubscriptions`
--

LOCK TABLES `chromaticfoldersubscriptions` WRITE;
/*!40000 ALTER TABLE `chromaticfoldersubscriptions` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticfoldersubscriptions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticgrouprepository`
--

DROP TABLE IF EXISTS `chromaticgrouprepository`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticgrouprepository` (
  `display_name` varchar(100) NOT NULL DEFAULT '',
  `domain` varchar(255) NOT NULL DEFAULT '',
  `group_type` varchar(20) NOT NULL DEFAULT '',
  `policy_moniker` varchar(255) DEFAULT 'NIL',
  `group_container` varchar(255) NOT NULL DEFAULT '',
  `group_chromatic` int(1) DEFAULT '0',
  `deleted` int(1) DEFAULT '0',
  `group_guid` varchar(255) DEFAULT NULL,
  `uSNChanged` int(11) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticgrouprepository`
--

LOCK TABLES `chromaticgrouprepository` WRITE;
/*!40000 ALTER TABLE `chromaticgrouprepository` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticgrouprepository` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticmessageflags`
--

DROP TABLE IF EXISTS `chromaticmessageflags`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticmessageflags` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `absolute_url_string` varchar(255) NOT NULL,
  `answered` int(1) NOT NULL DEFAULT '0',
  `deleted` int(1) NOT NULL DEFAULT '0' COMMENT '0',
  `draft` int(1) NOT NULL DEFAULT '0',
  `flagged` int(1) NOT NULL DEFAULT '0',
  `recent` int(1) NOT NULL DEFAULT '1',
  `seen` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`idx`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=294 COMMENT='InnoDB free: 9216 kB';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticmessageflags`
--

LOCK TABLES `chromaticmessageflags` WRITE;
/*!40000 ALTER TABLE `chromaticmessageflags` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticmessageflags` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticmessagefolderoffset`
--

DROP TABLE IF EXISTS `chromaticmessagefolderoffset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticmessagefolderoffset` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `absolute_url_string` varchar(255) NOT NULL,
  `folder_offset` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`idx`),
  UNIQUE KEY `absolute_url_string` (`absolute_url_string`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 4388864 kB';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticmessagefolderoffset`
--

LOCK TABLES `chromaticmessagefolderoffset` WRITE;
/*!40000 ALTER TABLE `chromaticmessagefolderoffset` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticmessagefolderoffset` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticserverrepository`
--

DROP TABLE IF EXISTS `chromaticserverrepository`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticserverrepository` (
  `mail_server` varchar(255) NOT NULL DEFAULT '',
  `policy_moniker` varchar(255) DEFAULT NULL,
  `domain` varchar(255) NOT NULL DEFAULT '',
  `server_container` varchar(255) NOT NULL DEFAULT '',
  `version` varchar(100) DEFAULT NULL,
  `private_size` varchar(20) DEFAULT NULL,
  `public_size` varchar(20) DEFAULT NULL,
  `mdb_size` varchar(20) DEFAULT NULL,
  `search_server` varchar(255) DEFAULT NULL,
  `search_port` int(11) DEFAULT '8002',
  `destination_folder` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticserverrepository`
--

LOCK TABLES `chromaticserverrepository` WRITE;
/*!40000 ALTER TABLE `chromaticserverrepository` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticserverrepository` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticstorerepository`
--

DROP TABLE IF EXISTS `chromaticstorerepository`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticstorerepository` (
  `domain` varchar(255) NOT NULL DEFAULT '',
  `mail_server` varchar(255) NOT NULL DEFAULT '',
  `storage_name` varchar(255) NOT NULL DEFAULT '',
  `store_name` varchar(255) NOT NULL DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticstorerepository`
--

LOCK TABLES `chromaticstorerepository` WRITE;
/*!40000 ALTER TABLE `chromaticstorerepository` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticstorerepository` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticsubscribefolders`
--

DROP TABLE IF EXISTS `chromaticsubscribefolders`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticsubscribefolders` (
  `idx` int(11) NOT NULL AUTO_INCREMENT,
  `folder_hierarchy` varchar(255) NOT NULL,
  `delimiter` varchar(20) NOT NULL DEFAULT '/',
  `folder_name` varchar(255) NOT NULL,
  `select_` int(1) NOT NULL DEFAULT '1',
  `inferiors` int(1) NOT NULL DEFAULT '1',
  `marked` int(1) NOT NULL DEFAULT '0',
  `public` int(1) NOT NULL DEFAULT '0',
  `touched` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`idx`),
  UNIQUE KEY `folder_name` (`folder_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=122 COMMENT='InnoDB free: 16384 kB';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticsubscribefolders`
--

LOCK TABLES `chromaticsubscribefolders` WRITE;
/*!40000 ALTER TABLE `chromaticsubscribefolders` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticsubscribefolders` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticuserextended`
--

DROP TABLE IF EXISTS `chromaticuserextended`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticuserextended` (
  `mail_default_folder` varchar(255) NOT NULL DEFAULT '',
  `mail_server` varchar(255) DEFAULT NULL,
  `mail_private_store` varchar(100) DEFAULT NULL,
  `mail_storage_group` varchar(255) NOT NULL DEFAULT '',
  `domain` varchar(255) NOT NULL DEFAULT '',
  `search_server` varchar(255) DEFAULT 'NIL',
  `search_port` int(11) DEFAULT '8002',
  `destination_folder` int(11) DEFAULT '0',
  `deleted` int(1) DEFAULT '0',
  `mailbox_guid` varchar(255) DEFAULT NULL,
  `uSNChanged` int(11) DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticuserextended`
--

LOCK TABLES `chromaticuserextended` WRITE;
/*!40000 ALTER TABLE `chromaticuserextended` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticuserextended` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `chromaticuserrepository`
--

DROP TABLE IF EXISTS `chromaticuserrepository`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `chromaticuserrepository` (
  `mail_nickname` varchar(100) NOT NULL DEFAULT '',
  `display_name` varchar(100) NOT NULL DEFAULT '',
  `mail_default_folder` varchar(255) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `policy_moniker` varchar(255) DEFAULT 'NIL',
  `mail_private_store` varchar(100) DEFAULT NULL,
  `mail_size` int(11) unsigned DEFAULT NULL,
  `mail_item_count` int(11) unsigned DEFAULT NULL,
  `mail_container` varchar(255) DEFAULT NULL,
  `mail_server` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`mail_default_folder`),
  UNIQUE KEY `mail_nickname` (`mail_nickname`),
  UNIQUE KEY `mail_default_folder` (`mail_default_folder`),
  UNIQUE KEY `DefaultFolder` (`mail_nickname`,`mail_default_folder`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `chromaticuserrepository`
--

LOCK TABLES `chromaticuserrepository` WRITE;
/*!40000 ALTER TABLE `chromaticuserrepository` DISABLE KEYS */;
/*!40000 ALTER TABLE `chromaticuserrepository` ENABLE KEYS */;
UNLOCK TABLES;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = latin1 */ ;
/*!50003 SET character_set_results = latin1 */ ;
/*!50003 SET collation_connection  = latin1_swedish_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
/*!50003 CREATE*/ /*!50017 DEFINER=`root`@`localhost`*/ /*!50003 TRIGGER `chromaticuserrepository_after_ins_tr` AFTER INSERT ON `chromaticuserrepository`   FOR EACH ROW BEGIN     DECLARE strNickname VARCHAR(100);         SELECT UCASE( NEW.mail_nickname ) INTO strNickname;          INSERT INTO chromaticsubscribefolders( folder_hierarchy ,                                       delimiter ,                                       folder_name ,                                       select_ ,                                       inferiors ,                                       marked ,                                       public )     VALUES (strNickname , '/' , CONCAT( '/' , strNickname , '/' ) , 0 , 1 , 0 , 0  ) ,                       (strNickname , '/' , CONCAT( '/' , strNickname , '/DELETED ITEMS' ) , 1 , 1 , 0 , 0 ) ,            (strNickname , '/' , CONCAT( '/' , strNickname , '/DRAFTS' ) , 1 , 1 , 0 , 0 ) ,               (strNickname , '/' , CONCAT( '/' , strNickname , '/SENT ITEMS' ) , 1 , 1 , 0 , 0 ) ,              (strNickname , '/' , CONCAT( '/' , strNickname , '/INBOX' ) , 1 , 1 , 0 , 0 ) ,            (strNickname , '/' , CONCAT( '/' , strNickname , '/CALENDAR' ) , 1 , 1 , 0 , 0 );             END */;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-06-24 22:42:42
