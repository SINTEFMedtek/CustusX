#!/bin/bash

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2011.10.14
#
# Description:
#
#	Install Github.com key
#
#####################################################


set -x
USER=`whoami`
ROOT=~

# these keys match christiana's github account

PRIVATE_KEY='-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAs75KzLA5a/AYrEIbHmZrjMTYAISyfcvyySTB2o+YOO5yL2Iw
+T+RekWvTDpn53h45k+Pm5uAz5QBn/EgVGvFMYFgAnNcTDfW34cqZjW8VLKzv8vH
uLL8v+PerMukTpZOXF8Ay7D6Kb5lHxht9qohGrcmygud1qxSNtcBeJuR3B/ie4bF
uCGDfPyqnFyl7WcZpwWVdPpY3775UQL5wCGPt7WOEs1LPnwE0B0E/izfqxXT1Z7T
g+xO6rdwSGQiiOAGUYE2FAsnEJ/EM9jqepb/u3Eba1UeVdGcX9DA2vt/E+MbIUXC
gH6kGXRapS1HJxpJbf5TV15q76SNeX/K/DWqiQIDAQABAoIBAQCpLFqUiuyQxftC
7jk+BKIHbSHsYvjoY7PQU5NNVcetFehM5rs3bPhMwBe8xGAVQZNxGcMNb2/ub9yT
oEyQiJqh/KXTyKY/anMIEdz7NGpcwZFi3LbPBU/Uf/GOa2DrQpbgY96I2Lqr3XKp
sMGW3wunbGjK0+IsMiZzTSwqSyoepiOct4VDgzpSrf/or4xjhxuMl4uKrgGSmBNj
BUOItBcIHPtx4aMckkh199u27ypUfv4Amu3aKFsy4TieQHnOzEW9NX8vrFt9bNQ8
WHhomqh+m9EvpKm/nZ9Rthma936+UnBqtN0jAvXZlxWdfdTA+bhhzsWxgh5PqLLw
6EFGqlwBAoGBAOHZRFleYHbWVzcXTIpCaeOHoyPSSixDyGoXyY95n3iK0jBJGUSA
YFkBxejy72ZnB5oK3QqnfD5tFTLr9aTyIqYEOZ7zHjIZIeGKOLtBHhXSE7oYRgJa
bM/mEKni71XS8XzNZ/SR67a39t16gwi2bA62B4baZNoZ7FJIUv8ayrAJAoGBAMu9
TU0XPGh1hcc6PpLxE5XJguPXWTRlkkXZz7oG3IivE8039V9BEVH8N0DnNUi+FHc1
iPHsXqTyzUrW0FhQdHQ6uQ+NN0Q4i62XtMZfSgUFGX/fXxIR7qZN6NTYyqJPGo2x
WpUYQSfKB/pT28rxj3/Z7/gznaR/6cQhnFibjMaBAoGBAJxaqPnh35CGBS0roR6a
qeoOTgAPprAMYSQpwa3lemSeXrN099w3r/wOwS/rd5eCPn4DhVHJ77bcF+//8Ti3
NQuQI0AKed4fDhixc2MXyrc6j9Om2Fjqtyga03B5nk3hUrOfFJf5qaD+rRZZqXuP
pzFsDbvrWCSyC6xPS742kxnZAoGAaqsJtbrcnbQp/ky4RiaBooKGTNOKmKw1EO0g
nLzK6aUAMQmDJgPjFZF/PDPQ6ezaui8FK+QdCS813GDrY9F7pv63tTSyIr2YI6Kx
uspY7pfk6dYA6G0LDXbs18NJFAFmXZjlW/n+4t4bnywYfwBJqR5pTQF5H474zP7r
5FhMNAECgYBnwDBPRx7Q/gC+iIdTTIKHj/RuC6FuYB93N/ayWvIiOgo/Dv7jvELp
SMRVHo2Y4BbJFUqg/CiInVb/9dmqa9OaN+OmtqLM5kShwPA82x8XjOBhSa0M3WsW
YkgByFOyr7WL13kuYIKczbPiETTUtIf58JotywHopGxyc7b0tDiAAw==
-----END RSA PRIVATE KEY-----'

KNOWN_HOSTS='github.com,207.97.227.239 ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEAq2A7hRGmdnm9tUDbO9IDSwBK6TbQa+PXYPCPy6rbTrTtw7PHkccKrpp0yVhp5HdEIcKr6pLlVDBfOLX9QUsyCOV0wzfjIJNlGEYsdlLJizHhbn2mUjvSAHQqZETYP81eFzLQNnPHt4EVVUh7VfDESU84KezmD5QlWpXLmvU31/yMf+Se8xhHTvKSCZIFImWwoG6mbUoWf9nzpIoaSjB+weqqUUmpaaasXVal72J+UX2B+2RPW3RcT0eOzQgqlJL3RKrTJvdsjE3JEAvGq3lGHSZXy28G3skua2SmVi/w4yCE6gbODqnTWlg7+wC604ydGXA8VJiS5ap43JXiUFFAaQ==
medtekserver.sintef.no,129.241.242.92 ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAQEA3JQpF1QFmSJmLuc1WhM0ggGuWJIc1p5dV1GdSC9v+TMZeVrOSu3ZaJqOeWPVshNhz9AWS0ltibKS3naIyoVanb5tzYRTxbF+RBXIRUc3mjuAPPT+PuXjosmXgI041QGyZop3Q0UgKE6TKl8FYa0jBaxIvfRpZFHEbvg9o/0nkV1vDRvAYb2kudlpZq1JMnH9yo5BKa1951bymI+I+MpKXrDYLes9j96e1/Dx6s3gCcQwjQDJtWA9cRMokIpY3xLpE6CO/+GUDneE0zoqwAheZjVsVM9tznEkQqlndjm+JC8/td4J44y1B+d2AMXRzyvzQ1cO1oiOFqOdylXsnSX31Q=='

mkdir -p ~/.ssh
echo "$PRIVATE_KEY" >> ~/.ssh/id_rsa
sudo chmod 600 ~/.ssh/id_rsa
echo "$KNOWN_HOSTS" >> ~/.ssh/known_hosts
sudo chmod 600 ~/.ssh/id_rsa


