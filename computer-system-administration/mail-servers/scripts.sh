#!/bin/bash

#firm:
apt install postfix

# choose internet site
# mail server name: firm.pg
# netplan set up:
sudo nano /etc/netplan/*.yaml 
addresses: [192.168.10.10/24]
sudo netplan apply
# postfix conf:
sudo nano /etc/postfix/main.cf
relay_domains = department.firm.pg
mynetworks += 192.168.10.11/24
sudo postfix reload

#department
apt install postfix

# choose satellite
# mial server name: department.firm.pg
# SMTP relay host: 192.168.10.10
# netplan set up:
sudo nano /etc/netplan/*.yaml 
addresses: [192.168.10.11/24]
sudo netplan apply
# postfix conf:
sudo nano /etc/postfix/main.cf
sudo postfix reload
