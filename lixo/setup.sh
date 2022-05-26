#!/bin/bash
read -e -r -p "Digite o seu usuário: " USER
read -e -r -p "Digite a sua senha: " PASSWORD
read -e -r -p "Digite a porta que você deseja usar (2210, 2220, 2230...2290, 22100): " PORT

export LASDPC_IP=andromeda.lasdpc.icmc.usp.br
export LASDPC_USER=${USER}
export LASDPC_PASS=${PASSWORD}
export LASDPC_PORT=${PORT}