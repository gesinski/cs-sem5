# Main and subordinate mail server

## Overview
The goal of this project is to create a company email system that handles mail communication within a firm. The system is designed to operate only inside the firm’s internal network.

The firm server is the central server, while each department has its own dedicated server. If an email is sent within the same department, the communication is handled by the department’s server. If an email is sent from one department to another, the message is routed through the firm server.

Since all communication occurs exclusively within the firm’s servers, no anti-spam filters are required.

## Technologies
Each server runs on Ubuntu Server 24.04 LTS. The main firm server uses Postfix, while the department servers use both Postfix and Dovecot to handle IMAP services.

Users needs postfix and can access their email using any mail user agent that supports IMAP. In this project, Mutt was used as the mail client.

## Installation

### Firm main server

```
apt install postfix ssmtp mialutils
```

### Department servers

```
apt install postfix ssmtp mialutils dovecot-core dovecot-imapd dovecot-lmtpd
```

### Users

```
apt install mailutils postfix mutt
```

## Configuration

All servers and clients must have a static IP address configured, for example using **Netplan**.

This can be done by editing the Netplan configuration file:

```
sudo nano /etc/netplan/*.yaml
```
Then, add the following line (the IP address should be diffrent for every machine) after the network interface section:

```
addresses: [192.168.10.10/24]
```

### Firm main server

#### Postfix configuration

First, the Postfix configuration file needs to be modified. This can be done by running:

```
sudo nano /etc/postfix/main.cf
```

In this file, add or update the following lines (if they already exist, modify them accordingly):

```
myhostname = firm.pg
mydestination = $myhostname, localhost.localdomain, localhost
mynetworks = 192.168.10.0/24
transport_maps = hash:/etc/postfix/transport
```

#### Transport map

Next, configure the transport map.

Open the transport configuration file:

```
nano /etc/postfix/transport
```

In the file, add the following entries:

```
department.firm.pg smtp:[department.firm.pg]
department2.firm.pg smtp:[department2.firm.pg]
```

To apply the changes, run:
```
postmap /etc/postfix/transport
sudo systemctl reload postfix
```

#### DNS

For the server to resolve the domains of other servers, DNS entries must be configured. 
Edit the hosts file:
``` 
sudo nano /etc/hosts/
```
Add the following lines as an example:
```
192.168.10.11 department.firm.pg
192.168.10.13 department2.firm.pg
```

### Department servers

#### Postfix configuration

As with the main server, the Postfix configuration must be modified:
```
sudo nano /etc/postfix/main.cf
```

In this file, add or update the following lines (modify them if they already exist):
```
myhostname = department.firm.pg
mydomain = firm.pg
mydestination = department.firm.pg, localhost.localdomain, localhost
transport_maps = hash:/etc/postfix/transport
mynetworks 192.168.10.0/24
virtual_mailbox_maps = hash:/etc/postfix/vmailbox
virtual_mailbox_base = /var/mail/vhosts
virtual_alias_maps = hash:/etc/postfix/virtual
virtual_uid_maps = static:5000
virtual_gid_maps = static:5000
```

In `/etc/postfix/master.cf`, uncomment the following lines:

```
submission inet n       -       n       -       -       smtpd
  -o syslog_name=postfix/submission
  -o smtpd_tls_security_level=encrypt
  -o smtpd_sasl_auth_enable=yes
  -o smtpd_client_restrictions=permit_sasl_authenticated,reject
```

Apply the changes:
```
sudo systemctl reload postfix
```

#### User mailboxes
To create user mailboxes, edit the virtual mailbox file:

```
nano /etc/postfix/vmailbox
```

Add entries for each user in the department, for example:

```
user1@department.firm.pg department/user1/
```

Create the mailbox directories and set permissions:

```
mkdir -p /var/mail/vhosts/department/user1
chown -R 5000:5000 /var/mail/vhosts
```

#### Transport map
Create or edit the transport map file:

```
nano /etc/postfix/transport
```

Configure all department domains to send mail via the main firm server:

```
department2.firm.pg smtp:[firm.pg]
```

Apply the transport map:

```
postmap /etc/postfix/transport
```

#### DNS
Edit the hosts file:
```
sudo nano /etc/hosts
```
Add the main server domain:

```
192.168.10.10 firm.pg
```

#### Dovecot Configuration

Edit the main Dovecot configuration file `/etc/dovecot/dovecot.conf`.
Add the following line:
```
protocols = imap lmtp
```
Open the mail configuration file:
```
nano /etc/dovecot/conf.d/10-mail.conf
```
Add the following lines:

```
!include auth-system.conf.ext
!include auth-passwdfile.conf.ext
```

In the authentication configuration file `/etc/dovecot/conf.d/10-auth.conf`.
Add the following line:
```
!include auth-passwdfile.conf.ext
```

Finally, edit the password file configuration:
```
nano /etc/dovecot/conf.d/auth-passwdfile.conf.ext
```
Configure it as follows:
```
passdb {
    args = scheme=SHA512-CRYPT /etc/dovecot/passwd
}

userdb {
    args = uid=5000 gid=5000
}
```

### Users

#### Postfix Configuration

In postfix main file `/etc/postfix/main.cf`.
Add or change following lines:
```
myhostname = user1
mydomain = department.firm.pg
relayhost = [192.168.10.11] #department ip
mynetworks = 192.168.10.0/24
```

#### DNS

Configure department domain in `/etc/hosts` file:

```
192.168.10.11 department.firm.pg
```

#### MUA Configuration

In the Mutt configuration file `~/.muttrc`, add the following lines (the password shown is an example).

```
set realname = "user1"
set from "user1@department.firm.pg"
set imap_user = "user1@department.firm.pg"
set imap_pass = "ask"
set folder = "imaps://department.firm.pg"
set spoolfile = "+INBOX"
set smtp_url = "smtp://user1@department.firm.pg@department.firm.pg:587/"
set smtp_pass = "ask"

set ssl_starttls = yes
set ssl_force_tls = no
```

## Usage

### Adding a User

To add a user, run one of the following commands on department server (the password will be prompted):

```
sudo echo "user1@department.firm.pg:$(doveadm pw -s SHA512-CRYPT)" | sudo tee -a /etc/dovecot/users
```
or
```
sudo echo "user1@department.firm.pg:$(doveadm pw -s SHA512-CRYPT)" | sudo tee -a /etc/dovecot/passwd
```

### Testing Authentication
To test user authentication, run:

```
sudo doveadm auth test user1@department.firm.pg ask
```

### Sending Emails
Emails are sent using Mutt.

To open Mutt on the user machine, run:
```
mutt
```

## Security
Passwords are encrypted using SHA-512, and all connections are secured via SSL.

## Testing
Mail traffic can be monitored using the following command:

```
tail /var/log/mail.log
```