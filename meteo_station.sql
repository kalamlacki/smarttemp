-- MariaDB dump 10.19  Distrib 10.11.6-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: 127.0.0.1    Database: meteo_station
-- ------------------------------------------------------
-- Server version	10.11.6-MariaDB-0+deb12u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `serial_numbers`
--
CREATE DATABASE meteo_station;
use meteo_station;

DROP TABLE IF EXISTS `serial_numbers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `serial_numbers` (
  `sernum` varchar(16) NOT NULL,
  `url` varchar(256) DEFAULT NULL,
  `mac` varchar(12) DEFAULT NULL,
  `smart_plug_url` varchar(256) DEFAULT NULL,
  `smart_plug_user_pass` varchar(128) DEFAULT NULL,
  `smart_plug_mac` varchar(12) DEFAULT NULL,
  `enabled` int(2) DEFAULT 0,
  `smart_plug_type` char(1) NOT NULL DEFAULT 'E',
  PRIMARY KEY (`sernum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;


DROP TABLE IF EXISTS `temperature_results`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature_results` (
  `sernum` varchar(16) NOT NULL,
  `temp` float NOT NULL,
  `readdate` datetime NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `smartplug_state` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `temperature_results_sernums_con` (`sernum`),
  KEY `readdateidx` (`readdate`),
  KEY `tempidx` (`temp`),
  KEY `sernumidx` (`sernum`)
) ENGINE=InnoDB AUTO_INCREMENT=47114449 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `temperature_results_hist`
--

DROP TABLE IF EXISTS `temperature_results_hist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature_results_hist` (
  `sernum` varchar(16) NOT NULL,
  `temp` float NOT NULL,
  `readdate` datetime NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `id` int(11) NOT NULL,
  `smartplug_state` tinyint(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `temperature_results_temp`
--

DROP TABLE IF EXISTS `temperature_results_temp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature_results_temp` (
  `sernum` varchar(16) NOT NULL,
  `temp` float NOT NULL,
  `readdate` datetime NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `id` int(11) NOT NULL DEFAULT 0,
  `created` datetime NOT NULL,
  `smartplug_state` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `temperature_results_view`
--

DROP TABLE IF EXISTS `temperature_results_view`;
/*!50001 DROP VIEW IF EXISTS `temperature_results_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `temperature_results_view` AS SELECT
 1 AS `sernum`,
  1 AS `temp`,
  1 AS `readdate`,
  1 AS `user_id`,
  1 AS `id` */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `temperature_schedule`
--

DROP TABLE IF EXISTS `temperature_schedule`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature_schedule` (
  `sernum` varchar(16) NOT NULL,
  `min_temp` float NOT NULL,
  `max_temp` float NOT NULL,
  `start_date` datetime NOT NULL,
  `stop_date` datetime NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `id` int(11) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`),
  KEY `temperature_schedule_fk_sn` (`sernum`),
  KEY `user_id` (`user_id`),
  CONSTRAINT `ts_sernum_fk` FOREIGN KEY (`sernum`) REFERENCES `serial_numbers` (`sernum`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=4622 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `temperature_schedule_view`
--

DROP TABLE IF EXISTS `temperature_schedule_view`;
/*!50001 DROP VIEW IF EXISTS `temperature_schedule_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `temperature_schedule_view` AS SELECT
 1 AS `sernum`,
  1 AS `min_temp`,
  1 AS `max_temp`,
  1 AS `start_date`,
  1 AS `stop_date`,
  1 AS `id` */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `temperature_schedule_weekly`
--

DROP TABLE IF EXISTS `temperature_schedule_weekly`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature_schedule_weekly` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(10) unsigned NOT NULL,
  `sernum` varchar(16) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL,
  `min_temp` float NOT NULL,
  `max_temp` float NOT NULL,
  `start_time` time NOT NULL,
  `stop_time` time NOT NULL,
  `weekday` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `sernum` (`sernum`),
  CONSTRAINT `tsw_sernum_fk` FOREIGN KEY (`sernum`) REFERENCES `serial_numbers` (`sernum`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=2521 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_polish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `username` varchar(200) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `passwd` varchar(32) CHARACTER SET latin1 COLLATE latin1_swedish_ci NOT NULL,
  `enabled` tinyint(4) NOT NULL DEFAULT 1,
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `webpage_counter`
--

DROP TABLE IF EXISTS `webpage_counter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `webpage_counter` (
  `counter` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3 COLLATE=utf8mb3_polish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
