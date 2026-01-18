#!/bin/bash

#firm:
apt install postfix

## choose internet site
## mail server name: firm.pg
## netplan set up:
sudo nano /etc/netplan/*.yaml 
#addresses: [192.168.10.10/24]
sudo netplan apply
## postfix conf:
sudo nano /etc/postfix/main.cf
#mynetworks += 192.168.10.0/24
sudo systemctl reload postfix
sudo systemctl restart postfix

postmap /etc/postfix/transport
nano /etc/postfix/transport

nano /etc/postfix/main.cf
##main.cf changes:
#myhostname = firm.pg
#mydestination = $myhostname, localhost.localdomain, localhost
#mynetworks = 192.168.10.0/24
#transport_maps = hash:/etc/postfix/transport

nano /etc/postfix/transport
#transport file configuration

#department.firm.pg smtp:[department.firm.pg]
#department2.firm.pg smtp:[department2.firm.pg]

postmap /etc/postfix/transport

#dns
sudo nano /etc/hosts/
#add lines:
#192.168.10.11 department.firm.pg
#192.168.10.13 department2.firm.pg

sudo apt install ssmtp
sudo apt install mialutils

echo "tekst" | mail -s "Naglowek" -a "FROM:adres_dostawcy" odbiorca
#ex. echo "tekst od firm" | mail -s "test" ask@department.firm.pg

tail /var/log/mail.log

#department
apt install postfix

# choose internet site
# mial server name: department.firm.pg
# netplan set up:
sudo nano /etc/netplan/*.yaml 
addresses: [192.168.10.11/24]
sudo netplan apply
# postfix conf:
sudo nano /etc/postfix/main.cf

# myhostname = department.firm.pg
# mydomain = firm.pg
# mydestination = department.firm.pg, localhost.localdomain, localhost
# transport_maps = hash:/etc/postfix/transport
# mynetworks 192.168.10.0/24
# virtual_mailbox_maps = hash:/etc/postfix/vmailbox
# virtual_mailbox_base = /var/mail/vhosts
# virtual_alias_maps = hash:/etc/postfix/virtual
# virtual_uid_maps = static:5000
# virtual_gid_maps = static:5000

#master.cf uncomment
sudo nano /etc/postfix/master.cf

# submission inet n       -       n       -       -       smtpd
#   -o syslog_name=postfix/submission
#   -o smtpd_tls_security_level=encrypt
#   -o smtpd_sasl_auth_enable=yes
#   -o smtpd_client_restrictions=permit_sasl_authenticated,reject

#add
nano /etc/postfix/vmailbox

#user1@department.firm.pg department/user1/

mkdir -p /var/mail/vhosts/department/user1
chown -R 5000:5000 /var/mail/vhosts

nano /etc/postfix/transport

# department2.firm.pg smtp:[firm.pg]

postmap /etc/postfix/transport

nano /etc/postfix/virtual

mkdir -p /var/mail/vhosts/department.firm.pg/user1
chown -R 5000:5000 /var/mail/vhosts

#dns 
sudo nano /etc/hosts

# 192.168.10.10 firm.pg

# dovecot conf
apt install dovecot-core dovecot-imapd dovecot-lmtpd

# add lines to
nano /etc/dovecot/dovecot.conf

#protocols = imap lmtp

# add lines to
nano /etc/dovecot/conf.d/10-mail.conf
#!include auth-system.conf.ext
#!include auth-passwdfile.conf.ext

#
nano /etc/dovecot/conf.d/10-auth.conf
#!include auth-passwdfile.conf.ext

# change
nano /etc/dovecot/conf.d/auth-passwdfile.conf.ext
# passdb {
#     args = scheme=SHA512-CRYPT /etc/dovecot/passwd
# }

# userdb {
#     args = uid=5000 gid=5000
#}

#adding user
sudo echo "user1@department.firm.pg:$(doveadm pw -s SHA512-CRYPT)" | sudo tee -a /etc/dovecot/users
#albo sudo echo "user1@department.firm.pg:$(doveadm pw -s SHA512-CRYPT)" | sudo tee -a /etc/dovecot/passwd

sudo doveadm auth test user1@department.firm.pg ask
sudo postconf -M submission

#user

sudo apt install mailutils postfix mutt

# netplan as before diffrent ip

# change or add
sudo nano /etc/postfix/main.cf
# myhostname = user1
# mydomain = department.firm.pg
# relayhost = [192.168.10.11] #department ip
# mynetworks = 192.168.10.0/24

# add
sudo nano /etc/hosts

# 192.168.10.11 department.firm.pg

#
nano ~/.muttrc

# set realname = "user1"
# set from "user1@department.firm.pg"
# set imap_user = "user1@department.firm.pg"
# set imap_pass = "ask"
# set folder = "imaps://department.firm.pg"
# set spoolfile = "+INBOX"
# set smtp_url = "smtp://user1@department.firm.pg@department.firm.pg:587/"
# set smtp_pass = "ask"

# set ssl_starttls = yes
# set ssl_force_tls = no

# sending mail:
# echo "mail body" | mial -s "mail title" -a "FROM:user1@department.firm.pg" user2@department.firm.pg
# or via mutt 