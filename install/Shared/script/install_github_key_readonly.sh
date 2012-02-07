#!/bin/bash

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2011.12.12
#
# Description:
#
#	Install Github.com readonly key
#
#####################################################


set -x
USER=`whoami`
ROOT=~

# these keys match the github account ChristianaOpen, 
# which is setup with pull only access to the CustusX/SSC repos.


PRIVATE_KEY='-----BEGIN RSA PRIVATE KEY-----
MIIEoQIBAAKCAQEAvdG5Hvj89kjiN7vgVRJFsnT9tOtTrp984Vp3KhA8kwNvgNb6
ZNdw6KuremF0GWABBxtW0WKBPbCPbe/guM5xCwntGEduKBkK/ssv8oWappXY41Mp
w6gr/fNlME6xVFIrjTIcFsWLBHwiUEAX/Vfxcd7ZBsn63hy3/+YKbZpRgFQEq6z5
DZdzhtewoNAFiQ7DNWiZaAdPUfFoEHg4JHs0E36rq55xrk0OwFHTG6bapCKfr/0g
6cI9wImekNH/gvtyhU91K8dRhrnWHo6/jwfe8EtQ5bmKgI7NYBz0VxbAjvKk5X6y
3QVkkfXv29KqtHodcoIb8+Ve0H79vQ2SizgX3wIBIwKCAQAVsZF/4fEGNDccT/xh
fm5dibaYVWimPhzmjf79fjLO+RQOuXuzwNm0MOBlwf6j0HUlYjXOyW3bKh8FP/xs
5GSwze81+Yjuo8a+CJfD8gMLuVqdoxq3Rmtteub+NOEQ83KxDQqGQnZJp8loQdbb
H/5WKBjNk20gstMzMDu0wS3ix5Oc0HQIEOETOaz+Jt1VYV6+FuozxEJ1PWuOKsN8
WIb8RxjXDXe7ofcqBhsbBTDrXQi7lei+z/jhYimo8blpArJNL+1pP2sF33YoO0Is
9rSEIIJwWYPYTeBQHWXdfl1e8mTwEoXkeSXgh39LpQukKu9tPQlCcGwRkYPWXBiJ
CSGrAoGBAOLZSPpFX/NUaJoasy4nRCvFZ3TjyCNr6asxbJ/JN4Tvk+cBnHxpUbVj
Hp/8rmSHQL48+aI5eL167VPFbUK4zVXAjtVVjy0RN+0MDB4Gn/Pq+eirv03XzsGB
c7WM9S3hbZx63p1obekbAH8MQA9mv8EyH1WX87ZgNC6vNFwt5+UdAoGBANY2RAeB
o87KNmzWSMa2tppwuKM/SeZkLk/DFWnbPRIEfKtQTB2bMiatfcXyiwoRc9UJo8oq
rb6RgV5FIaZzVoShXUJ9TTwCHp8yfB5++T+BNPLi6vsEFqEhqAN4D+GuvBU1/gHl
c3AKU9b5rnVxQq1EjLVWmfbWkY16M1+uQEwrAoGAeyV/Y0o7ZtYM5fE8sqeaCSIE
9k/EawARImtJmJHNrpCvYCVyNOi3U9a42oHbA19dqRnJWBCSAHXujI+++FWxS87u
c89cWkss/QaKPC97ZyfJfk6bDQAfyCGz15WpqzE7gNT8gVXyhdt1TEh6jAST+ynW
fuwWljQ5la+Rc95n8WcCgYA3FUSxeRt+UUEx7fV07SekDltdLYgICyHZ/vbgtLfu
sLJX8BOSlZ8uhGIrliPAwqjBwKZ10HXR6ubAeC05bh2P0bjlNinNmiUhn0R83tJg
yXQD8TUb+bytF0h166xP+btV6U/x2+p75WYEFFFuqBh1soqVB6PvA/mKx6bPdfNG
yQKBgQDYpatNFCKiUxe2Ez0n3eSC/X+KS5edTg83kGudpw7efjWh4YWqPuPpb1an
oMEvM+KkbM8PpFY+Fzo1cP0uGjavWh92/3uDX2JMr6Meubuat+DVzIFrCJuBQoK7
2zrspjuDN1IQeustNRje4WYE6ZZXqxZjkiKKxgomj/t/EpQTPw==
-----END RSA PRIVATE KEY-----'



KNOWN_HOSTS='github.com,207.97.227.239 ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAq2A7hRGmdnm9tUDbO9IDSwBK6TbQa+PXYPCPy6rbTrTtw7PHkccKrpp0yVhp5HdEIcKr6pLlVDBfOLX9QUsyCOV0wzfjIJNlGEYsdlLJizHhbn2mUjvSAHQqZETYP81eFzLQNnPHt4EVVUh7VfDESU84KezmD5QlWpXLmvU31/yMf+Se8xhHTvKSCZIFImWwoG6mbUoWf9nzpIoaSjB+weqqUUmpaaasXVal72J+UX2B+2RPW3RcT0eOzQgqlJL3RKrTJvdsjE3JEAvGq3lGHSZXy28G3skua2SmVi/w4yCE6gbODqnTWlg7+wC604ydGXA8VJiS5ap43JXiUFFAaQ==
medtekserver.sintef.no,129.241.242.92 ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA3JQpF1QFmSJmLuc1WhM0ggGuWJIc1p5dV1GdSC9v+TMZeVrOSu3ZaJqOeWPVshNhz9AWS0ltibKS3naIyoVanb5tzYRTxbF+RBXIRUc3mjuAPPT+PuXjosmXgI041QGyZop3Q0UgKE6TKl8FYa0jBaxIvfRpZFHEbvg9o/0nkV1vDRvAYb2kudlpZq1JMnH9yo5BKa1951bymI+I+MpKXrDYLes9j96e1/Dx6s3gCcQwjQDJtWA9cRMokIpY3xLpE6CO/+GUDneE0zoqwAheZjVsVM9tznEkQqlndjm+JC8/td4J44y1B+d2AMXRzyvzQ1cO1oiOFqOdylXsnSX31Q=='

mkdir -p ~/.ssh

echo "$PRIVATE_KEY" >> ~/.ssh/id_rsa
sudo chmod 600 ~/.ssh/id_rsa
echo "$KNOWN_HOSTS" >> ~/.ssh/known_hosts
sudo chmod 600 ~/.ssh/id_rsa


