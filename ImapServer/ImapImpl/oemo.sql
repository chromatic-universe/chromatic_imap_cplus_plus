# SQL Manager 2007 for MySQL 4.1.2.1
# ---------------------------------------
# Host     : keren
# Port     : 3306
# Database : oemo


SET FOREIGN_KEY_CHECKS=0;

CREATE DATABASE `oemo`
    CHARACTER SET 'latin1'
    COLLATE 'latin1_swedish_ci';

USE `oemo`;

#
# Structure for the `oemoarchivehash` table : 
#

CREATE TABLE `oemoarchivehash` (
  `idx` int(11) NOT NULL auto_increment,
  `absolute_url_string` varchar(255) NOT NULL,
  `relative_url_string` varchar(255) NOT NULL default '',
  `mbx` varchar(255) NOT NULL default '',
  `has_attachments` int(11) NOT NULL default '0',
  `msg_size` int(11) NOT NULL default '0',
  `delivery_date` varchar(100) NOT NULL default '',
  `archived` int(1) NOT NULL default '0',
  `pending` int(1) default '0',
  PRIMARY KEY  (`idx`),
  UNIQUE KEY `absolute_url_string` (`absolute_url_string`),
  UNIQUE KEY `idx` (`idx`),
  KEY `Mailbox` (`mbx`)
) ENGINE=MyISAM AUTO_INCREMENT=243 DEFAULT CHARSET=latin1;

CREATE TRIGGER `oemoarchivehash_before_ins_tr` BEFORE INSERT ON `oemoarchivehash`
  FOR EACH ROW
BEGIN
	DECLARE strFolder VARCHAR( 255 );
    DECLARE strSubFolder VARCHAR( 255 );     
    
    SELECT NEW.mbx INTO strFolder;
    
    SELECT SUBSTRING( strFolder , INSTR( strFolder , 'MBX' ) + LENGTH( 'MBX' ) )
    INTO strSubFolder;     
    SELECT SUBSTRING( strSubFolder , INSTR( strSubFolder , '/' ) + LENGTH( '/' ) - 1 )
    INTO strSubFolder;      
    
    
    UPDATE oemosubscribefolders
    SET marked = 1
    WHERE folder_name = strSubFolder;   
    
    INSERT INTO oemomessageflags( absolute_url_string ,
    				              answered ,
    					          deleted ,
    							  draft ,
    					          flagged ,
    						      recent ,
    			                  seen )
           						  VALUES( NEW.absolute_url_string ,
                                          0 ,
                                          0 ,
                                          0 ,
                                          0 ,
                                          1 ,
                                          0 );
                                          
    
    
END;

CREATE TRIGGER `oemoarchivehash_after_upd_tr` AFTER UPDATE ON `oemoarchivehash`
  FOR EACH ROW
BEGIN
   IF ( OLD.archived = 0 ) AND ( NEW.archived = 1 ) THEN
     INSERT INTO oemomessagefolderoffset ( absolute_url_string ,
                                          folder_offset )
                                          VALUES( NEW.absolute_url_string ,
                                          DATE_FORMAT( NOW() , '%m%d%y') );
   END IF; 
END;

CREATE TRIGGER `oemoarchivehash_before_del_tr` AFTER DELETE ON `oemoarchivehash`
  FOR EACH ROW
BEGIN
    DELETE FROM oemomessagefolderoffset
    WHERE oemomessagefolderoffset.absolute_url_string = OLD.absolute_url_string;
    
    DELETE FROM oemomessageflags
    WHERE oemomessageflags.absolute_url_string = OLD.absolute_url_string;
END;

#
# Structure for the `oemodomainrepository` table : 
#

CREATE TABLE `oemodomainrepository` (
  `domain_name` varchar(255) NOT NULL default '',
  `mail_server` varchar(255) default NULL,
  UNIQUE KEY `domain_name` (`domain_name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

#
# Structure for the `oemoflashmessage` table : 
#

CREATE TABLE `oemoflashmessage` (
  `id` int(11) NOT NULL auto_increment,
  `moniker` varchar(100) NOT NULL default '',
  `permanent_url` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`,`moniker`)
) ENGINE=MyISAM AUTO_INCREMENT=986 DEFAULT CHARSET=latin1;

#
# Structure for the `oemofoldersubscriptions` table : 
#

CREATE TABLE `oemofoldersubscriptions` (
  `subscription_idx` int(11) NOT NULL auto_increment,
  `idx` int(11) NOT NULL,
  `mail_nickname` varchar(255) NOT NULL,
  PRIMARY KEY  (`subscription_idx`),
  UNIQUE KEY `idx_subscribe` (`idx`,`mail_nickname`),
  KEY `idx` (`idx`)
) ENGINE=InnoDB AUTO_INCREMENT=25 DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=16384 COMMENT='InnoDB free: 4388864 kB';

#
# Structure for the `oemogrouprepository` table : 
#

CREATE TABLE `oemogrouprepository` (
  `display_name` varchar(100) NOT NULL default '',
  `domain` varchar(255) NOT NULL default '',
  `group_type` varchar(20) NOT NULL default '',
  `policy_moniker` varchar(255) default 'NIL',
  `group_container` varchar(255) NOT NULL default '',
  `group_oemo` int(1) default '0',
  `deleted` int(1) default '0',
  `group_guid` varchar(255) default NULL,
  `uSNChanged` int(11) default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

#
# Structure for the `oemomessageflags` table : 
#

CREATE TABLE `oemomessageflags` (
  `idx` int(11) NOT NULL auto_increment,
  `absolute_url_string` varchar(255) NOT NULL,
  `answered` int(1) NOT NULL default '0',
  `deleted` int(1) NOT NULL default '0' COMMENT '0',
  `draft` int(1) NOT NULL default '0',
  `flagged` int(1) NOT NULL default '0',
  `recent` int(1) NOT NULL default '1',
  `seen` int(1) NOT NULL default '0',
  PRIMARY KEY  (`idx`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=294 COMMENT='InnoDB free: 9216 kB';

#
# Structure for the `oemomessagefolderoffset` table : 
#

CREATE TABLE `oemomessagefolderoffset` (
  `idx` int(11) NOT NULL auto_increment,
  `absolute_url_string` varchar(255) NOT NULL,
  `folder_offset` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`idx`),
  UNIQUE KEY `absolute_url_string` (`absolute_url_string`)
) ENGINE=InnoDB AUTO_INCREMENT=13 DEFAULT CHARSET=latin1 COMMENT='InnoDB free: 4388864 kB';

#
# Structure for the `oemoserverrepository` table : 
#

CREATE TABLE `oemoserverrepository` (
  `mail_server` varchar(255) NOT NULL default '',
  `policy_moniker` varchar(255) default NULL,
  `domain` varchar(255) NOT NULL default '',
  `server_container` varchar(255) NOT NULL default '',
  `version` varchar(100) default NULL,
  `private_size` varchar(20) default NULL,
  `public_size` varchar(20) default NULL,
  `mdb_size` varchar(20) default NULL,
  `search_server` varchar(255) default NULL,
  `search_port` int(11) default '8002',
  `destination_folder` int(11) default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

#
# Structure for the `oemostorerepository` table : 
#

CREATE TABLE `oemostorerepository` (
  `domain` varchar(255) NOT NULL default '',
  `mail_server` varchar(255) NOT NULL default '',
  `storage_name` varchar(255) NOT NULL default '',
  `store_name` varchar(255) NOT NULL default ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

#
# Structure for the `oemosubscribefolders` table : 
#

CREATE TABLE `oemosubscribefolders` (
  `idx` int(11) NOT NULL auto_increment,
  `folder_hierarchy` varchar(255) NOT NULL,
  `delimiter` varchar(20) NOT NULL default '/',
  `folder_name` varchar(255) NOT NULL,
  `select_` int(1) NOT NULL default '1',
  `inferiors` int(1) NOT NULL default '1',
  `marked` int(1) NOT NULL default '0',
  `public` int(1) NOT NULL default '0',
  `touched` int(1) NOT NULL default '0',
  PRIMARY KEY  (`idx`),
  UNIQUE KEY `folder_name` (`folder_name`)
) ENGINE=InnoDB AUTO_INCREMENT=27458 DEFAULT CHARSET=latin1 AVG_ROW_LENGTH=122 COMMENT='InnoDB free: 16384 kB';

#
# Structure for the `oemouserextended` table : 
#

CREATE TABLE `oemouserextended` (
  `mail_default_folder` varchar(255) NOT NULL default '',
  `mail_server` varchar(255) default NULL,
  `mail_private_store` varchar(100) default NULL,
  `mail_storage_group` varchar(255) NOT NULL default '',
  `domain` varchar(255) NOT NULL default '',
  `search_server` varchar(255) default 'NIL',
  `search_port` int(11) default '8002',
  `destination_folder` int(11) default '0',
  `deleted` int(1) default '0',
  `mailbox_guid` varchar(255) default NULL,
  `uSNChanged` int(11) default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

#
# Structure for the `oemouserrepository` table : 
#

CREATE TABLE `oemouserrepository` (
  `mail_nickname` varchar(100) NOT NULL default '',
  `display_name` varchar(100) NOT NULL default '',
  `mail_default_folder` varchar(255) character set latin1 collate latin1_bin NOT NULL default '',
  `policy_moniker` varchar(255) default 'NIL',
  `mail_private_store` varchar(100) default NULL,
  `mail_size` int(11) unsigned default NULL,
  `mail_item_count` int(11) unsigned default NULL,
  `mail_container` varchar(255) default NULL,
  `mail_server` varchar(255) default NULL,
  PRIMARY KEY  (`mail_default_folder`),
  UNIQUE KEY `mail_nickname` (`mail_nickname`),
  UNIQUE KEY `mail_default_folder` (`mail_default_folder`),
  UNIQUE KEY `DefaultFolder` (`mail_nickname`,`mail_default_folder`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TRIGGER `oemouserrepository_after_ins_tr` AFTER INSERT ON `oemouserrepository`
  FOR EACH ROW
BEGIN
    DECLARE strNickname VARCHAR(100);    
    SELECT UCASE( NEW.mail_nickname ) INTO strNickname;
    
    INSERT INTO oemosubscribefolders( folder_hierarchy ,
                                      delimiter ,
                                      folder_name ,
                                      select_ ,
                                      inferiors ,
                                      marked ,
                                      public )
    VALUES (strNickname , '/' , CONCAT( '/' , strNickname , '/' ) , 0 , 1 , 0 , 0  ) ,           
           (strNickname , '/' , CONCAT( '/' , strNickname , '/DELETED ITEMS' ) , 1 , 1 , 0 , 0 ) ,
           (strNickname , '/' , CONCAT( '/' , strNickname , '/DRAFTS' ) , 1 , 1 , 0 , 0 ) ,   
           (strNickname , '/' , CONCAT( '/' , strNickname , '/SENT ITEMS' ) , 1 , 1 , 0 , 0 ) ,  
           (strNickname , '/' , CONCAT( '/' , strNickname , '/INBOX' ) , 1 , 1 , 0 , 0 ) ,
           (strNickname , '/' , CONCAT( '/' , strNickname , '/CALENDAR' ) , 1 , 1 , 0 , 0 );
           
END;

#
# Definition for the `FolderChildByName` function : 
#

CREATE DEFINER = 'root'@'localhost' FUNCTION `FolderChildByName`(folder_name vARCHAR(255))
    RETURNS varchar(255) CHARSET latin1
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
	
    DECLARE strFolderName VARCHAR( 255 );                  
           
    SELECT SUBSTRING_INDEX( folder_name , '/' , -1 ) INTO strFolderName; 
        
  RETURN ( strFolderName );
END;

#
# Definition for the `FolderParentByPosition` function : 
#

CREATE DEFINER = 'root'@'localhost' FUNCTION `FolderParentByPosition`(folder_name VARCHAR(255), nesting_level INTEGER(11))
    RETURNS varchar(255) CHARSET latin1
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    DECLARE strTemp VARCHAR( 255 );         
    DECLARE dwIdx INT DEFAULT 0;
    DECLARE dwQdx INT DEFAULT 0;
    SELECT SUBSTRING_INDEX( folder_name , '/' , nesting_level ) INTO strTemp;        
    SELECT LEFT( strTemp , LENGTH( strTemp ) ) INTO strTemp;
    SELECT LOCATE( strTemp , folder_name ) INTO dwQdx;   
    SELECT SUBSTRING( folder_name , 1 , dwQdx - 1 ) INTO strTemp;       
     
  RETURN ( strTemp );
END;

#
# Definition for the `copy_messages_from_subscribe_folder` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `copy_messages_from_subscribe_folder`(IN from_folder_name VARCHAR(255), IN to_folder_name VARCHAR(255), IN mail_nickname VARCHAR(100), IN message_list VARCHAR(1024))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
	 
    
	DECLARE strContext VARCHAR( 255 );   
    DECLARE strCopy VARCHAR( 255 );  
    DECLARE strMoniker VARCHAR( 10 );
    DECLARE dwPos INT DEFAULT 1;
    DECLARE dwMax INT DEFAULT 0;
    DECLARE dwMin INT DEFAULT 0; 
    
    DECLARE atom VARCHAR(50);
    DECLARE source_string VARCHAR(1024);
    DECLARE dwIdx INT DEFAULT 1;
    DECLARE dwQdx INT DEFAULT 1;
    
    SET source_string = message_list;	                                 
    SET strMoniker = '/MBX/';
    
    SELECT oemouserrepository.mail_default_folder INTO strContext
    FROM   oemouserrepository
	WHERE  oemouserrepository.mail_nickname = mail_nickname;
    
    SELECT LOCATE( strMoniker , strContext ) INTO dwPos;    
    SELECT LEFT( strContext , LENGTH( strMoniker ) - 1 + ( dwPos - 1) ) INTO strContext;    
    SELECT CONCAT( strContext , to_folder_name ) INTO strCopy;
    
     WHILE ( dwIdx != 0 ) DO
    	 
    	SELECT LOCATE( ',' , source_string ) INTO dwIdx;
		IF dwIdx = 0 THEN
        	SET atom = source_string;            
        ELSE        
        	SET atom = SUBSTR( source_string , dwQdx , dwIdx - 1 ); 
        	SET source_string = SUBSTR( source_string FROM dwIdx + 1 );        	       	
		END IF;
        
        UPDATE oemoarchivehash
        SET oemoarchivehash.mbx = to_folder_name
        WHERE oemoarchivehash.mbx = from_folder_name
        AND oemoarchivehash.idx = CAST( atom AS SIGNED );
    
    END WHILE;  
    
    
    
    COMMIT;
    
    
    
END;

#
# Definition for the `delete_from_subscribe_folders` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `delete_from_subscribe_folders`(IN folder_name VARCHAR(255))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
     
    
	DELETE FROM oemosubscribefolders 
    WHERE oemosubscribefolders.folder_name = folder_name;
    
    COMMIT;
    
END;

#
# Definition for the `delete_subscription` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `delete_subscription`(IN folder_name VARCHAR(255), IN nickname VARCHAR(100), OUT been_deleted INTEGER(1))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
     
    
    DECLARE dwIdx INT DEFAULT 0; 
    
    SELECT 
  		oemosubscribefolders.idx  INTO dwIdx
	FROM
  		oemosubscribefolders
  	INNER JOIN oemofoldersubscriptions ON (oemosubscribefolders.idx = oemofoldersubscriptions.idx)
    WHERE oemosubscribefolders.folder_name = folder_name 
    AND   oemofoldersubscriptions.mail_nickname = nickname;
             
   
    
    DELETE FROM oemofoldersubscriptions 
    WHERE oemofoldersubscriptions.idx = dwIdx     
    AND oemofoldersubscriptions.mail_nickname = nickname;            
    
        
  	COMMIT;      
	
       
END;

#
# Definition for the `enumerate_messages_by_folder` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `enumerate_messages_by_folder`(IN folder_name vARCHAR(255))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    SELECT oemoarchivehash.absolute_url_string ,
           oemoarchivehash.idx ,
           oemoarchivehash.msg_size ,
           oemomessagefolderoffset.folder_offset ,
           oemomessageflags.deleted ,
           oemomessageflags.answered ,
           oemomessageflags.draft ,
           oemomessageflags.flagged , .
           oemomessageflags.recent ,
           oemomessageflags.seen 
	FROM
  		oemoarchivehash
  		INNER JOIN oemomessagefolderoffset ON (oemoarchivehash.absolute_url_string
        = oemomessagefolderoffset.absolute_url_string) 
        LEFT OUTER JOIN oemomessageflags ON
        (oemomessagefolderoffset.absolute_url_string = oemomessageflags.absolute_url_string)
   	WHERE oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) ) AND
          oemoarchivehash.archived = 1
    ORDER BY oemoarchivehash.idx; 
    
END;

#
# Definition for the `enum_imap_message_set` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `enum_imap_message_set`(IN dwLow INTEGER(11), IN dwHigh INTEGER(11))
    DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
             
    
	SET @dw = 0;
    SELECT  CAST( @dw := @dw + 1 AS CHAR )  AS message_number , idx AS uid
    FROM oemoarchivehash
    GROUP BY message_number HAVING CAST( message_number AS SIGNED ) BETWEEN dwLow
    AND dwHigh;
    
END;

#
# Definition for the `expunge_messages_by_idx` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `expunge_messages_by_idx`(IN message_list vARCHAR(1024))
    NOT DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
     
    
    
    
    DECLARE atom VARCHAR(50);
    DECLARE source_string VARCHAR(1024);
    DECLARE dwIdx INT DEFAULT 1;
    DECLARE dwQdx INT DEFAULT 1;
    
    SET source_string = message_list;                                        
    
     WHILE ( dwIdx != 0 ) DO
         
        SELECT LOCATE( ',' , source_string ) INTO dwIdx;
        IF dwIdx = 0 THEN
            SET atom = source_string;            
        ELSE        
            SET atom = SUBSTR( source_string , dwQdx , dwIdx - 1 ); 
            SET source_string = SUBSTR( source_string FROM dwIdx + 1 );                       
        END IF;
        
        DELETE FROM oemoarchivehash
        WHERE oemoarchivehash.idx = CAST( atom AS SIGNED );
    
    END WHILE;  
    
    
    
    COMMIT;
    
    
    
END;

#
# Definition for the `get_mail_folders_by_user_moniker` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_mail_folders_by_user_moniker`(IN user_moniker VARCHAR(50))
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
	         
    
	DECLARE str VARCHAR( 100 ); 
    
    SELECT CONCAT( '%' , user_moniker , '%' ) INTO str;    
    
    SELECT DISTINCT SUBSTR( (TRIM( TRAILING '/' FROM mbx )) , INSTR( mbx , UCASE( user_moniker ) ) )
    AS ImapFolderList
    FROM oemoarchivehash
	WHERE mbx LIKE UCASE( str );
    
END;

#
# Definition for the `get_mail_folders_by_user_moniker_ex` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_mail_folders_by_user_moniker_ex`(IN mail_nickname VARCHAR(100))
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
      
      SELECT 
  			oemosubscribefolders.folder_name,
  			oemosubscribefolders.select_,
  			oemosubscribefolders.inferiors,
  			oemosubscribefolders.marked,
  			oemosubscribefolders.public
FROM
  oemosubscribefolders 
WHERE oemosubscribefolders.folder_hierarchy = mail_nickname;
END;

#
# Definition for the `get_message_flags_by_folder` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_message_flags_by_folder`(folder_name VARCHAR(50), OUT out_count_of_answered INTEGER(11), OUT out_count_of_seen INTEGER(11), OUT out_count_of_deleted INTEGER(11), OUT out_count_of_draft INTEGER(11), OUT out_count_of_flagged INTEGER(11), OUT out_count_of_recent INTEGER(11), OUT uidvalidity INTEGER(11), OUT uidnext INTEGER(11), OUT first_unseen INTEGER(11))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY INVOKER
    COMMENT ''
BEGIN
        
    
    
    
    DECLARE dwAnsweredCount INT DEFAULT 0;
    DECLARE dwDeletedCount INT DEFAULT 0;   
    DECLARE dwDraftCount INT DEFAULT 0;
    DECLARE dwFlaggedCount INT DEFAULT 0;
    DECLARE dwRecentCount INT DEFAULT 0;  
    DECLARE dwSeenCount  INT DEFAULT 0; 
    DECLARE dwLoopEnd INT DEFAULT 0;          
    DECLARE dwValidity  INT DEFAULT 0;  
    DECLARE dwMax  INT DEFAULT 0;    
    DECLARE dwFirstUnseen INT DEFAULT 0;
    DECLARE dwCounter INT DEFAULT 0;     
	
    
    DECLARE cur1 CURSOR FOR             
    SELECT  oemomessageflags.seen   	   
	FROM
  			oemoarchivehash
			INNER JOIN oemomessageflags 
            ON (oemoarchivehash.absolute_url_string = oemomessageflags.absolute_url_string)
	WHERE
			oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) )
            AND oemoarchivehash.archived = 1;   
     
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET dwLoopEnd = 1;   
    
    
  	SELECT 	SUM(oemomessageflags.seen)  , 
		    SUM(oemomessageflags.recent)  , 
			SUM(oemomessageflags.answered) , 
			SUM(oemomessageflags.draft ) , 
		    SUM(oemomessageflags.flagged ) ,  
		    SUM(oemomessageflags.deleted) 
    INTO 	dwSeenCount ,
    		dwRecentCount  ,    		 
            dwAnsweredCount ,
            dwDraftCount ,
            dwFlaggedCount ,
            dwDeletedCount
	FROM
  			oemoarchivehash
			INNER JOIN oemomessageflags 
            ON (oemoarchivehash.absolute_url_string = oemomessageflags.absolute_url_string)
	WHERE
			oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) )
            AND oemoarchivehash.archived = 1;
    
	SET out_count_of_answered = dwAnsweredCount;
    SET out_count_of_seen =  dwSeenCount;   
    SET out_count_of_deleted =  dwDeletedCount;   
    SET out_count_of_draft=  dwDraftCount;   
    SET out_count_of_flagged =  dwFlaggedCount;   
    SET out_count_of_recent =  dwRecentCount; 
                    
    
    SELECT oemosubscribefolders.idx INTO dwValidity
    FROM oemosubscribefolders
    WHERE oemosubscribefolders.folder_name = folder_name;
     
    SET uidvalidity = dwValidity; 
     
         
    
    SELECT MAX( oemoarchivehash.idx ) INTO dwMax
    FROM oemoarchivehash;
    
    SET uidnext = dwMax + 1; 
    
    
    
    OPEN cur1;  
    flags_loop: LOOP
    	FETCH cur1 INTO dwFirstUnseen;      				 
                      
    	IF dwLoopEnd = 1 THEN       
         LEAVE flags_loop;
    	END IF;
      
    	SET dwCounter = dwCounter + 1;				     	
        
        IF dwFirstUnseen = 0 THEN
         
           	SET first_unseen = dwCounter;  
            LEAVE flags_loop;
                
        END IF;     
      
    END LOOP flags_loop;     
    CLOSE cur1; 
    
     
      
END;

#
# Definition for the `get_message_flags_by_folder_ex` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `get_message_flags_by_folder_ex`(IN folder_name VARCHAR(50))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
        
    
    
    
    DECLARE dwAnsweredCount INT DEFAULT 0;
    DECLARE dwDeletedCount INT DEFAULT 0;   
    DECLARE dwDraftCount INT DEFAULT 0;
    DECLARE dwFlaggedCount INT DEFAULT 0;
    DECLARE dwRecentCount INT DEFAULT 0;  
    DECLARE dwSeenCount  INT DEFAULT 0; 
    DECLARE dwLoopEnd INT DEFAULT 0;          
    DECLARE dwValidity  INT DEFAULT 0;  
    DECLARE dwMax  INT DEFAULT 0;    
    DECLARE dwFirstUnseen INT DEFAULT 0;
    DECLARE dwCounter INT DEFAULT 0;     
    DECLARE dwTotal INT DEFAULT 0;     
    DECLARE dwUidnext INT DEFAULT 0;     
    DECLARE first_unseen INT DEFAULT 0;     
	
    
    DECLARE cur1 CURSOR FOR             
    SELECT  oemomessageflags.seen   	   
	FROM
  			oemoarchivehash
			INNER JOIN oemomessageflags 
            ON (oemoarchivehash.absolute_url_string = oemomessageflags.absolute_url_string)
	WHERE
			oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) )
            AND oemoarchivehash.archived = 1;   
     
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET dwLoopEnd = 1;
    
    SELECT COUNT(*) 
    oemoarchivehash INTO dwTotal  
    FROM oemoarchivehash
    WHERE  oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) )
    AND oemoarchivehash.archived = 1;
    
    
  	SELECT 	SUM(oemomessageflags.seen)  , 
		    SUM(oemomessageflags.recent)  , 
			SUM(oemomessageflags.answered) , 
			SUM(oemomessageflags.draft ) , 
		    SUM(oemomessageflags.flagged ) ,  
		    SUM(oemomessageflags.deleted) 
    INTO 	dwSeenCount ,
    		dwRecentCount  ,    		 
            dwAnsweredCount ,
            dwDraftCount ,
            dwFlaggedCount ,
            dwDeletedCount
	FROM
  			oemoarchivehash
			INNER JOIN oemomessageflags 
            ON (oemoarchivehash.absolute_url_string = oemomessageflags.absolute_url_string)
	WHERE
			oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) )
            AND oemoarchivehash.archived = 1;
    
	
                    
    
    SELECT oemosubscribefolders.idx INTO dwValidity
    FROM oemosubscribefolders
    WHERE oemosubscribefolders.folder_name = folder_name;  
    
     
         
    
    SELECT MAX( oemoarchivehash.idx ) INTO dwMax
    FROM oemoarchivehash;
    
    SET dwUidnext = dwMax + 1; 
    
    
    
    OPEN cur1;  
    flags_loop: LOOP
        FETCH cur1 INTO dwFirstUnseen;                       
                      
        IF dwLoopEnd = 1 THEN       
         LEAVE flags_loop;
        END IF;
      
        SET dwCounter = dwCounter + 1;                         
        
        IF dwFirstUnseen = 0 THEN
         
               SET first_unseen = dwCounter;  
            LEAVE flags_loop;
                
        END IF;     
      
    END LOOP flags_loop;     
    CLOSE cur1;  
           
    SELECT  dwTotal , 
            dwValidity ,
            dwUidnext ,
      first_unseen ,     
      dwAnsweredCount ,
     dwSeenCount ,   
      dwDeletedCount ,   
      dwDraftCount ,  
      dwFlaggedCount ,  
      dwRecentCount;    
    
    
     
      
END;

#
# Definition for the `get_message_flags_by_url` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_message_flags_by_url`(IN url vARCHAR(255))
    DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
			SELECT  oemomessageflags.answered,
  					oemomessageflags.deleted,
  					oemomessageflags.draft,
  					oemomessageflags.flagged,
  					oemomessageflags.recent,
  					oemomessageflags.seen
			FROM
  					oemomessageflags 
            WHERE   oemomessageflags.absolute_url_string = url;
END;

#
# Definition for the `get_message_flags_by_user` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_message_flags_by_user`(IN mail_nickname VARCHAR(50), OUT out_count_of_answered INTEGER(11), OUT out_count_of_seen INTEGER(11), OUT out_count_of_deleted INTEGER(11), OUT out_count_of_draft INTEGER(11), OUT out_count_of_flagged INTEGER(11), OUT out_count_of_recent INTEGER(11))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY INVOKER
    COMMENT ''
BEGIN
        
    
    
    
    DECLARE dwAnswered INT;
    DECLARE dwDeleted INT;
    DECLARE dwDraft INT;
    DECLARE dwFlagged INT;
    DECLARE dwRecent INT;
    DECLARE dwSeen  INT;
    DECLARE dwAnsweredCount INT DEFAULT 0;
    DECLARE dwDeletedCount INT DEFAULT 0;   
    DECLARE dwDraftCount INT DEFAULT 0;
    DECLARE dwFlaggedCount INT DEFAULT 0;
    DECLARE dwRecentCount INT DEFAULT 0;  
    DECLARE dwSeenCount  INT DEFAULT 0; 
    DECLARE dwLoopEnd INT DEFAULT 0; 
    
    DECLARE cur1 CURSOR FOR             
    SELECT  oemomessageflags.answered ,
    		oemomessageflags.seen ,
            oemomessageflags.deleted ,
            oemomessageflags.draft ,
            oemomessageflags.flagged ,
            oemomessageflags.recent 
            
    	   
	FROM
  		oemoarchivehash
  		INNER JOIN oemouserrepository ON (oemoarchivehash.mbx = oemouserrepository.mail_default_folder)
  		INNER JOIN oemomessageflags ON (oemoarchivehash.absolute_url_string = oemomessageflags.absolute_url_string)  		
        AND oemouserrepository.mail_nickname = mail_nickname
        	FOR UPDATE ;      
     
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET dwLoopEnd = 1;    
    
    OPEN cur1;  
    flags_loop: LOOP
      FETCH cur1 INTO dwAnswered ,
      				  dwSeen ,
      				  dwDeleted ,
                      dwDraft ,
                      dwFlagged ,
                      dwRecent;
                      
      IF dwLoopEnd = 1 THEN       
         LEAVE flags_loop;
      END IF;
      				  
      IF NOT dwLoopEnd THEN  
      
         SET dwAnsweredCount = dwAnsweredCount + dwAnswered;
         SET dwSeenCount = dwSeenCount + dwSeen;            
      	 SET dwDeletedCount = dwDeletedCount + dwDeleted;
      	 SET dwDraftCount = dwDraftCount + dwDraft;
      	 SET dwFlaggedCount = dwFlaggedCount + dwFlagged;
      	 SET dwRecentCount = dwRecentCount + dwRecent;
      	 
      
      END IF;      
      
    END LOOP flags_loop;     
    CLOSE cur1; 
    
	SET out_count_of_answered = dwAnsweredCount;    
    SET out_count_of_seen =  dwSeenCount;   
    SET out_count_of_deleted =  dwDeletedCount;   
    SET out_count_of_draft=  dwDraftCount;   
    SET out_count_of_flagged =  dwFlaggedCount;   
    SET out_count_of_recent =  dwRecentCount;   
    
END;

#
# Definition for the `get_subcribe_folder_attributes` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_subcribe_folder_attributes`(IN folder_name VARCHAR(255), OUT isSelected INTEGER(11), OUT isMarked INTEGER(11), OUT isPublic INTEGER(11), OUT isInferiors INTEGER(11))
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
        
		SELECT oemosubscribefolders.select_ ,
               oemosubscribefolders.marked ,
               oemosubscribefolders.public ,
               oemosubscribefolders.inferiors
        INTO   isSelected ,
        	   isMarked ,
               isPublic ,
               isInferiors 
        FROM   oemosubscribefolders
        WHERE  oemosubscribefolders.folder_name = folder_name;
END;

#
# Definition for the `get_subscribe_folders_by_user` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_subscribe_folders_by_user`(IN mail_nickname VARCHAR(255))
    NOT DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
  
  
  SELECT 
  			CONCAT( '~' , oemosubscribefolders.delimiter , oemosubscribefolders.folder_hierarchy ,
  			oemosubscribefolders.folder_name , oemosubscribefolders.delimiter ) AS folder_name ,
  			oemosubscribefolders.select_,
  			oemosubscribefolders.inferiors,
  			oemosubscribefolders.marked,
  			oemosubscribefolders.public
FROM     oemouserrepository
  INNER JOIN oemofoldersubscriptions ON (oemouserrepository.mail_nickname = oemofoldersubscriptions.mail_nickname)
  INNER JOIN oemosubscribefolders ON (oemofoldersubscriptions.subscription_idx = oemosubscribefolders.idx) 
  WHERE oemouserrepository.mail_nickname = mail_nickname;
  
END;

#
# Definition for the `get_user_domain_name` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `get_user_domain_name`(IN mail_nickname VARCHAR(100), OUT domain_name VARCHAR(100))
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
     SELECT oemouserextended.domain INTO domain_name
     FROM  oemouserrepository
         INNER JOIN oemouserextended ON 
         (oemouserrepository.mail_default_folder = oemouserextended.mail_default_folder)
     WHERE oemouserrepository.mail_nickname = mail_nickname;
    
END;

#
# Definition for the `get_user_domain_name_ex` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `get_user_domain_name_ex`(IN mail_nickname VARCHAR(100))
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN       

          
     SELECT oemouserextended.domain AS domain_name
     FROM  oemouserrepository
         INNER JOIN oemouserextended ON 
         (oemouserrepository.mail_default_folder = oemouserextended.mail_default_folder)
     WHERE oemouserrepository.mail_nickname = mail_nickname;
     
     
    
END;

#
# Definition for the `get_user_mail_server` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `get_user_mail_server`(IN mail_nickname VARCHAR(255), OUT server_name VARCHAR(255))
    DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    DECLARE server VARCHAR(255);
    
    SELECT oemouserrepository.mail_server INTO server
    FROM  oemouserrepository 
    WHERE oemouserrepository.mail_nickname = mail_nickname;
    
    SET server_name = server;
    
END;

#
# Definition for the `get_user_mail_server_ex` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `get_user_mail_server_ex`(IN mail_nickname VARCHAR(255))
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    DECLARE server VARCHAR(255);
    
    SELECT oemouserrepository.mail_server INTO server
    FROM  oemouserrepository 
    WHERE oemouserrepository.mail_nickname = mail_nickname;
    
    SELECT server;
    
END;

#
# Definition for the `get_user_monikers` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `get_user_monikers`()
    NOT DETERMINISTIC
    READS SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
                   
      
      SELECT mail_nickname , display_name FROM oemouserrepository;         
      
      
END;

#
# Definition for the `has_new_messages_by_folder` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `has_new_messages_by_folder`(IN folder_name VARCHAR(255), INOUT vCount INT)
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
          DECLARE dwCount INT DEFAULT 0;
          SELECT COUNT( oemoarchivehash.idx ) INTO dwCount
            FROM
                  oemoarchivehash
                  INNER JOIN oemomessagefolderoffset ON (oemoarchivehash.absolute_url_string
                = oemomessagefolderoffset.absolute_url_string) 
                INNER JOIN oemomessageflags ON
                (oemomessagefolderoffset.absolute_url_string = oemomessageflags.absolute_url_string)
          WHERE oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) );
          IF dwCount > vCount THEN
             SET vCount = dwCount - vCount;
          ELSE
             SET vCount = 0;          
          END IF;
          
END;

#
# Definition for the `has_new_messages_by_folder_ex` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `has_new_messages_by_folder_ex`(IN folder_name VARCHAR(255), IN vCount INTEGER(11))
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
          DECLARE dwCount INT DEFAULT 0;
          DECLARE msg_count INT DEFAULT 0;
          SELECT COUNT( oemoarchivehash.idx ) INTO dwCount
            FROM
                  oemoarchivehash
                  INNER JOIN oemomessagefolderoffset ON (oemoarchivehash.absolute_url_string
                = oemomessagefolderoffset.absolute_url_string) 
                INNER JOIN oemomessageflags ON
                (oemomessagefolderoffset.absolute_url_string = oemomessageflags.absolute_url_string)
          WHERE oemoarchivehash.mbx LIKE ( CONCAT( '%/MBX' , folder_name ) );
          IF dwCount > vCount THEN
             SET msg_count = dwCount - vCount;
          ELSE
             SET msg_count = 0;          
          END IF; 
          
          SELECT msg_count;
END;

#
# Definition for the `insert_into_archivehash` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `insert_into_archivehash`(IN absolute_url VARCHAR(255), IN relative_url VARCHAR(255), IN mbx VARCHAR(255), IN d_date vARCHAR(255), IN size INTEGER(11))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
	INSERT INTO oemoarchivehash( absolute_url_string ,
    		                     relative_url_string ,
                                 mbx ,
                                 delivery_date ,
                                 msg_size ,
                                 archive ,
                                 pending  )
                                 VALUES( absolute_url ,
                                         relative_url ,
                                         mbx , 
                                         d_date ,
                                         size ,
                                         1 ,
                                         0 );
                                         
    COMMIT;
    
    
END;

#
# Definition for the `insert_into_subscribe_folders` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `insert_into_subscribe_folders`(IN in_hierarchy VARCHAR(255), IN in_delimiter VARCHAR(20), IN in_folder_name VARCHAR(255), IN in_select INTEGER(1), IN in_inferiors INTEGER(1), IN in_marked INTEGER(1), IN in_public INTEGER(1))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
	INSERT INTO oemosubscribefolders( folder_hierarchy ,
    						          delimiter ,
    								  folder_name ,
    								  select_ , 
    								  inferiors ,
    							      marked ,
    								  public )
    VALUES ( in_hierarchy , 
             in_delimiter ,
             in_folder_name ,
             in_select ,
             in_inferiors ,
             in_marked ,
             in_public );
             
    COMMIT;
             
    
END;

#
# Definition for the `insert_subscription` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `insert_subscription`(IN folder_name VARCHAR(255), IN nickname VARCHAR(100))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
	
	DECLARE dwIdx INT DEFAULT 0;
    DECLARE dwContinue INT DEFAULT  0;                   
    
    DECLARE CONTINUE HANDLER FOR NOT FOUND SET dwContinue = 1;    
    
    SELECT oemosubscribefolders.idx INTO dwIdx
    FROM oemosubscribefolders
    WHERE oemosubscribefolders.folder_name = folder_name;
    
    INSERT INTO oemofoldersubscriptions ( idx , mail_nickname )
    VALUES ( dwIdx , nickname );    
    
    SELECT dwIdx;
        
    COMMIT;      
   
END;

#
# Definition for the `rename_subscribe_folders` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `rename_subscribe_folders`(IN folder_name VARCHAR(255), IN new_name VARCHAR(255), IN hierarchy VARCHAR(255))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
    DECLARE strTemp VARCHAR( 255 ); 
    DECLARE strFolder VARCHAR( 255 );     
    DECLARE strChild VARCHAR( 255 ); 
    DECLARE dwIdx INT DEFAULT 0;
    DECLARE dwQdx INT DEFAULT 0;
    DECLARE dwLoopEnd INT DEFAULT 0;     
    
   
    
    UPDATE oemosubscribefolders
    SET    oemosubscribefolders.folder_name  =
           CONCAT( new_name , '/' , SUBSTRING_INDEX( oemosubscribefolders.folder_name , '/' , -1 ) )     	      
   	WHERE  oemosubscribefolders.folder_name LIKE CONCAT( '%' , folder_name , '/%' );
    
    
    UPDATE oemosubscribefolders
    SET    oemosubscribefolders.folder_name = new_name      
   	WHERE  oemosubscribefolders.folder_name = folder_name;    
    
    COMMIT;              
    
    
END;

#
# Definition for the `subscribe_folder_exists` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `subscribe_folder_exists`(IN folder_name VARCHAR(255))
    NOT DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
   
    
	DECLARE strTemp VARCHAR( 255 ); 
    DECLARE dwIdx INT DEFAULT 0;
    DECLARE dwQdx INT DEFAULT 0;
    DECLARE does_exist INT DEFAULT 0;   
         
    
    SELECT SUBSTRING_INDEX( folder_name , '/' , -1 ) INTO strTemp;    
    SELECT CONCAT( '/' , strTemp , '/' ) INTO strTemp; 
    SELECT STRCMP( '//' , strTemp) INTO dwQdx;
    SELECT LEFT( strTemp , LENGTH( strTemp ) - 1 ) INTO strTemp;  
    
    IF ( dwQdx <> 0 ) THEN     
    
    	             
        SELECT LOCATE( strTemp , folder_name ) INTO dwQdx;   
        SELECT SUBSTRING( folder_name , 1 , dwQdx - 1 ) INTO strTemp;                
        
        SELECT COUNT( oemosubscribefolders.idx ) INTO dwIdx	   
    	FROM oemosubscribefolders
   		WHERE oemosubscribefolders.folder_name LIKE ( CONCAT( strTemp , '%' ) )     
	     FOR UPDATE;    
            
        
        IF( dwIdx = 0 ) THEN 
        	SET does_exist = -1;
        ELSE
        	SELECT COUNT( oemosubscribefolders.idx ) INTO dwIdx	   
    		FROM oemosubscribefolders
    		WHERE oemosubscribefolders.folder_name = folder_name          
     		 FOR UPDATE; 
             
            SET does_exist = dwIdx;
            SELECT does_exist AS folder_exists;  
                    
    	END IF; 
         
                        
    ELSE
    	SELECT COUNT( oemosubscribefolders.idx ) INTO dwIdx	   
    	FROM oemosubscribefolders
    	WHERE oemosubscribefolders.folder_name = folder_name          
     	 FOR UPDATE; 
         
        SET does_exist = dwIdx;
        SELECT does_exist AS folder_exists;      
    	              
    END IF;  
    
    
     
END;

#
# Definition for the `update_archive_message` procedure : 
#

CREATE DEFINER = 'overtone'@'%' PROCEDURE `update_archive_message`(IN url VARCHAR(255))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
     UPDATE oemoarchivehash
     SET oemoarchivehash.archived = 1 ,
         oemoarchivehash.pending = 0 
     WHERE oemoarchivehash.absolute_url_string = url;
     
     
     COMMIT;
END;

#
# Definition for the `update_message_flags` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `update_message_flags`(IN url VARCHAR(255), IN isAnswered INTEGER(11), IN isDeleted INTEGER(11), IN isDraft INTEGER(11), IN isFlagged INTEGER(11), IN isRecent INTEGER(11), IN isSeen INTEGER(11))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    UPDATE oemomessageflags
    SET oemomessageflags.answered = isAnswered , 
    	oemomessageflags.deleted = isDeleted ,
        oemomessageflags.draft = isDraft ,
        oemomessageflags.flagged = isFlagged ,
        oemomessageflags.recent = isRecent ,
        oemomessageflags.seen = isSeen 
    WHERE oemomessageflags.absolute_url_string = url;
    
    COMMIT;
END;

#
# Definition for the `update_subscribe_folder_attributes` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `update_subscribe_folder_attributes`(IN folder_name VARCHAR(255), IN inSelectable INTEGER(11), IN inInferiors INTEGER(11), IN inMarked INTEGER(11), IN inPublic INTEGER(11))
    DETERMINISTIC
    MODIFIES SQL DATA
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
    
    
	UPDATE oemosubscribefolders
    SET oemosubscribefolders.select_ = inSelectable ,
        oemosubscribefolders.inferiors = inInferiors ,
        oemosubscribefolders.marked = inMarked ,
        oemosubscribefolders.public = inPublic
    WHERE oemosubscribefolders.folder_name = folder_name;
    
    COMMIT;
END;

#
# Definition for the `user_is_subscribed_to_folder` procedure : 
#

CREATE DEFINER = 'root'@'localhost' PROCEDURE `user_is_subscribed_to_folder`(IN folder_name VARCHAR(255), IN mail_nickname VARCHAR(100))
    DETERMINISTIC
    CONTAINS SQL
    SQL SECURITY DEFINER
    COMMENT ''
BEGIN
      
    
	  DECLARE local_exists INT DEFAULT 0;
          
	  SELECT  COUNT( oemosubscribefolders.idx ) AS does_exist
	  FROM    oemosubscribefolders
  	  INNER JOIN oemofoldersubscriptions ON (oemosubscribefolders.idx = oemofoldersubscriptions.idx) 
	  WHERE oemosubscribefolders.folder_name = folder_name AND
	  oemofoldersubscriptions.mail_nickname = mail_nickname;   
	  
      
END;

