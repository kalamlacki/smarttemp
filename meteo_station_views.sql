CREATE OR REPLACE
ALGORITHM = UNDEFINED VIEW `meteo_station`.`temperature_schedule_view` AS
select
    `w`.`sernum` AS `sernum`,
    `w`.`min_temp` AS `min_temp`,
    `w`.`max_temp` AS `max_temp`,
    cast(current_timestamp() as date) + interval `w`.`start_time` hour_second AS `start_date`,
    cast(current_timestamp() as date) + interval `w`.`stop_time` hour_second + interval 1 second AS `stop_date`,
    `w`.`id` AS `id`
from
    `meteo_station`.`temperature_schedule_weekly` `w`
where
    `w`.`weekday` = dayofweek(current_timestamp())
    and current_timestamp() >= cast(current_timestamp() as date) + interval `w`.`start_time` hour_second
    and current_timestamp() < cast(current_timestamp() as date) + interval `w`.`stop_time` hour_second + interval 1 second
    and ! exists(
    select
        1
    from
        `meteo_station`.`temperature_schedule` `s`
    where
        `s`.`sernum` = `w`.`sernum`
        and current_timestamp() >= `s`.`start_date`
        and current_timestamp() < `s`.`stop_date`
    limit 1)
union all
select
    `u`.`sernum` AS `sernum`,
    `u`.`min_temp` AS `min_temp`,
    `u`.`max_temp` AS `max_temp`,
    `u`.`start_date` AS `start_date`,
    `u`.`stop_date` AS `stop_date`,
    `u`.`id` AS `id`
from
    `meteo_station`.`temperature_schedule` `u`
where
    current_timestamp() >= `u`.`start_date`
    and current_timestamp() < `u`.`stop_date`;


