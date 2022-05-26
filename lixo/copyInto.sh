#!/bin/bash
ssh-keyscan -p $LASDPC_PORT -H $LASDPC_IP >> ~/.ssh/known_hosts
read -e -r -p "Entre o nome dos arquivos da maquina local: " ARQUIVOS
sshpass -p "$LASDPC_PASS" scp -P $LASDPC_PORT $ARQUIVOS $LASDPC_USER@$LASDPC_IP:/home/$LASDPC_USER