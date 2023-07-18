#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exitWithError(MYSQL *connection);
void checkResult(MYSQL_RES *result, MYSQL *connection);
void establishConnection(MYSQL *connection);
void showHome(MYSQL *connection);
void showLogin(MYSQL *connection);
unsigned char checkLogin(char login[9], MYSQL *connection);
unsigned char checkPassword(char senha[9], MYSQL *connection);
void showMenuProfessor(char ID_Professor[6], MYSQL *connection);
void selectSubject(char ID_Professor[6], MYSQL *connection);
void selectClass(char ID_Professor[6], char Nome_Materia[40], MYSQL *connection);
void showMainMenu(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void addTableSpaces(MYSQL_ROW row, unsigned char spaces, unsigned char fields);
void showTable(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void addGrades(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void showTopAverages(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void addStudent(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void removeStudent(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);
void writeCSVFile(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection);

int main() {
    MYSQL *connection = mysql_init(NULL);

    establishConnection(connection);

    showHome(connection);

    mysql_close(connection);
    exit(0);
}

void exitWithError(MYSQL *connection) {
    fprintf(stderr, "%s\n", mysql_error(connection));
    mysql_close(connection);
    exit(0);
}

void checkResult(MYSQL_RES *result, MYSQL *connection) {
    if(result == NULL) {
        printf("RESULT IS NULL");
        mysql_close(connection);
        exit(0);
    }
}

void establishConnection(MYSQL *connection) {
    if(connection == NULL) {
        fprintf(stderr, "%s\n", mysql_error(connection));
        exit(1);
    } else {
        printf("Inicializacao bem sucedida!\n");
        Sleep(2000);
    }

    char *host = "localhost";
    char *user = "root";
    char *password = "";
    char *database = "bd_ProjetoAV3";

    if(mysql_real_connect(connection, host, user, password, database, 3306, NULL, 0) == NULL) {
        exitWithError(connection);
    } else {
        printf("Conexao bem sucedida!\n");
        Sleep(2000);
        system("cls");
    }
}

void showHome(MYSQL *connection) {
    char choice;

    printf("Home\n");
    printf("1 - Fazer login\n");
    printf("2 - Sair\n");
    scanf("%c", &choice);

    switch(choice) {
    case '1':
        system("cls");
        showLogin(connection);
        break;
    case '2':
        mysql_close(connection);
        system("cls");
        exit(0);
        break;
    default:
        system("cls");
        showHome(connection);
        break;
    }
}

void showLogin(MYSQL *connection) {
    char query[81] = "SELECT ID_Professor, Nome_Professor FROM tbl_Professores WHERE Login = '";
    char login[9];
    char senha[9];
    char choice;

    system("cls");
    printf("Login Professor\n");
    printf("Login: ");
    scanf("%s", &login);
    printf("Senha: ");
    scanf("%s", &senha);

    if(!(checkLogin(login, connection) && checkPassword(senha, connection))) {
        printf("Login ou senha incorreta.\n");
        printf("1 - Tentar novamente\n");
        printf("2 - Voltar\n");
        scanf("%s", &choice);

        switch(choice) {
        case '1':
            showLogin(connection);
            break;
        default:
            showHome(connection);
        }
    } else {
        strcat(query, login);
        strcat(query, "'");

        if(mysql_query(connection, query)) {
            exitWithError(connection);
        }

        MYSQL_RES *result = mysql_store_result(connection);

        checkResult(result, connection);

        MYSQL_ROW row = mysql_fetch_row(result);

        system("cls");
        printf("Bem vindo professor %s", row[1] ? row[1] : NULL);
        Sleep(2000);
        system("cls");
        row[0] ? showMenuProfessor(row[0], connection) : "Falha ao carregar dados";
    }
}

unsigned char checkLogin(char login[9], MYSQL *connection) {
    char query[68] = "SELECT Login FROM tbl_Professores WHERE Login = '";

    strcat(query, login);
    strcat(query, "'");

    if(mysql_query(connection, query)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    MYSQL_ROW row = mysql_fetch_row(result);

    if(!row) {
        mysql_free_result(result);
        return 0;
    } else {
        mysql_free_result(result);
        return 1;
    }
}

unsigned char checkPassword(char senha[9], MYSQL *connection) {
    char query[68] = "SELECT Senha FROM tbl_Professores WHERE Senha = '";

    strcat(query, senha);
    strcat(query, "'");

    if(mysql_query(connection, query)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    MYSQL_ROW row = mysql_fetch_row(result);

    if(!row) {
        mysql_free_result(result);
        return 0;
    } else {
        mysql_free_result(result);
        return 1;
    }
}

void showMenuProfessor(char ID_Professor[6], MYSQL *connection) {
    char choice;

    printf("Menu Professor\n");
    printf("1 - Selecionar disciplina\n");
    printf("2 - Sair\n");
    scanf("%s", &choice);

    switch(choice) {
    case '1':
        selectSubject(ID_Professor, connection);
        break;
    case '2':
        showLogin(connection);
        break;
    default:
        showMenuProfessor(ID_Professor, connection);
    }
}

void selectSubject(char ID_Professor[6], MYSQL *connection) {
    char tmp;
    scanf("%c", &tmp);

    char query_materias[78] = "SELECT Nome_Materia FROM tbl_Professores_Materias WHERE ID_Professor = ";
    strcat(query_materias, ID_Professor);

    if(mysql_query(connection, query_materias)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    unsigned int fields = mysql_num_fields(result);

    MYSQL_ROW row;

    system("cls");
    printf("Selecionar Disciplina\n");

    while(row = mysql_fetch_row(result)) {
        for(int i = 0; i < fields; i++) {
            printf("%s\n", row[i] ? row[i] : NULL);
        }
    }

    mysql_free_result(result);

    char choice;
    char materia[40];
    printf("\nDisciplina: ");
    scanf("%[^\n]%*c", materia);

    char query_check_materia[101] = "SELECT Nome_Materia FROM tbl_Materias WHERE Nome_Materia = '";

    strcat(query_check_materia, materia);
    strcat(query_check_materia, "'");

    if(mysql_query(connection, query_check_materia)) {
        exitWithError(connection);
    }

    result = mysql_store_result(connection);

    checkResult(result, connection);

    row = mysql_fetch_row(result);

    if(row) {
        mysql_free_result(result);
        selectClass(ID_Professor, materia, connection);
    } else {
        mysql_free_result(result);
        printf("\nDisciplina nao encontrada\n");
        printf("1 - Tentar novamente\n");
        printf("2 - Voltar\n");
        scanf("%s", &choice);

        switch(choice) {
        case '1':
            system("cls");
            selectSubject(ID_Professor, connection);
            break;
        default:
            showMenuProfessor(ID_Professor, connection);
            break;
        }
    }
}

void selectClass(char ID_Professor[6], char Nome_Materia[40], MYSQL *connection) {
    char query_turmas[73] = "SELECT Nome_Turma FROM tbl_Professores_Turmas WHERE ID_Professor = ";
    strcat(query_turmas, ID_Professor);

    if(mysql_query(connection, query_turmas)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    unsigned int fields = mysql_num_fields(result);

    MYSQL_ROW row;

    system("cls");
    printf("Selecionar Turma\n");

    while(row = mysql_fetch_row(result)) {
        for(int i = 0; i < fields; i++) {
            printf("%s\n", row[i] ? row[i] : NULL);
        }
    }

    mysql_free_result(result);

    char choice;
    char turma[7];
    printf("\nTurma: ");
    scanf("%s", &turma);

    char query_check_turma[62] = "SELECT Nome_Turma FROM tbl_Turmas WHERE Nome_Turma = '";

    strcat(query_check_turma, turma);
    strcat(query_check_turma, "'");

    if(mysql_query(connection, query_check_turma)) {
        exitWithError(connection);
    }

    result = mysql_store_result(connection);

    checkResult(result, connection);

    row = mysql_fetch_row(result);

    if(row) {
        mysql_free_result(result);
        showMainMenu(Nome_Materia, turma, connection);
    } else {
        mysql_free_result(result);
        printf("\nTurma nao encontrada\n");
        printf("1 - Tentar novamente\n");
        printf("2 - Voltar\n");
        scanf("%s", &choice);

        switch(choice) {
        case '1':
            system("cls");
            selectClass(ID_Professor, Nome_Materia, connection);
            break;
        default:
            selectSubject(ID_Professor, connection);
        }
    }
}

void showMainMenu(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char choice;

    system("cls");
    printf("%s %s\n", Nome_Materia, Nome_Turma);
    printf("1 - Ver tabela\n");
    printf("2 - Adicionar aluno\n");
    printf("3 - Remover aluno\n");
    printf("4 - Exportar arquivo .csv\n");
    printf("5 - Sair\n");
    scanf("%s", &choice);

    switch(choice) {
        case '1':
            system("cls");
            showTable(Nome_Materia, Nome_Turma, connection);
            break;
        case '2':
            system("cls");
            addStudent(Nome_Materia, Nome_Turma, connection);
            break;
        case '3':
            system("cls");
            removeStudent(Nome_Materia, Nome_Turma, connection);
            break;
        case '4':
            system("cls");
            writeCSVFile(Nome_Materia, Nome_Turma, connection);
            break;
        case '5':
            system("cls");
            showHome(connection);
            break;
        default:
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
    }
}

void addTableSpaces(MYSQL_ROW row, unsigned char spaces, unsigned char i) {
    if(row[i]) {
        for(int j = 0; j < (spaces - strlen(row[i])); j++) {
            printf(" ");
        }
    } else {
        for(int j = 0; j < (spaces - strlen("  -  ")); j++) {
            printf(" ");
        }
    }
}

void showTable(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    printf("+------+---------------------------------------+---------------------------------------+------+-----+-----+-----+-----+\n");
    printf("|Matr  |Nome                                   |Curso                                  |Turma |1N   |2N   |3N   |Media|\n");
    printf("+------+---------------------------------------+---------------------------------------+------+-----+-----+-----+-----+\n");
    char query_matricula_nome[73] = "SELECT Matricula, Nome_Aluno FROM tbl_Alunos WHERE Nome_Turma = '";
    strcat(query_matricula_nome, Nome_Turma);
    strcat(query_matricula_nome, "'");

    if(mysql_query(connection, query_matricula_nome)) {
        exitWithError(connection);
    }

    MYSQL_RES *result_matricula_nome = mysql_store_result(connection);

    checkResult(result_matricula_nome, connection);

    unsigned int fields_matricula_nome = mysql_num_fields(result_matricula_nome);

    MYSQL_ROW row_matricula_nome;

    char query_curso[50] = "SELECT Curso FROM tbl_Turmas WHERE Nome_Turma = '";
    strcat(query_curso, Nome_Turma);
    strcat(query_curso, "'");

    if(mysql_query(connection, query_curso)) {
        exitWithError(connection);
    }

    MYSQL *result_curso = mysql_store_result(connection);

    checkResult(result_curso, connection);

    MYSQL_ROW row_curso = mysql_fetch_row(result_curso);

    char query_notas[134];
    char matricula[6];
    unsigned char spaces = 7;
    while(row_matricula_nome = mysql_fetch_row(result_matricula_nome)) {
        strcpy(query_notas, "SELECT Primeira_Nota, Segunda_Nota, Terceira_Nota, Media FROM tbl_Alunos_Materias WHERE Matricula = ");
        spaces = 6;
        for(unsigned char i = 0; i < fields_matricula_nome; i++) {
            printf("|%s", row_matricula_nome[i] ? row_matricula_nome[i] : "  -  ");
            addTableSpaces(row_matricula_nome, spaces, i);
            strcpy(matricula, row_matricula_nome[0]);

            i == 0 ? spaces = 39 : NULL;
        }

        printf("|%s", row_curso[0] ? row_curso[0] : "  -  ");
        addTableSpaces(row_curso, spaces, 0);

        printf("|%s", Nome_Turma ? Nome_Turma : "  -  ");

        strcat(query_notas, matricula);
        strcat(query_notas, " AND Nome_Materia = '");
        strcat(query_notas, Nome_Materia);
        strcat(query_notas, "'");

        if(mysql_query(connection, query_notas)) {
            exitWithError(connection);
        }

        MYSQL *result_notas = mysql_store_result(connection);

        checkResult(result_notas, connection);

        unsigned int fields_notas = mysql_num_fields(result_notas);

        MYSQL_ROW row_notas = mysql_fetch_row(result_notas);

        spaces = 5;
        for(unsigned char i = 0; i < fields_notas; i++) {
            printf("|%s", row_notas[i] ? row_notas[i] : "  -  ");
            addTableSpaces(row_notas, spaces, i);
        }

        mysql_free_result(result_notas);

        printf("|");
        printf("\n");
    }

    mysql_free_result(result_matricula_nome);
    mysql_free_result(result_curso);

    printf("+------+---------------------------------------+---------------------------------------+------+-----+-----+-----+-----+\n");

    char choice;

    printf("1 - Modificar notas\n");
    printf("2 - Ver 10 melhores medias\n");
    printf("3 - Voltar\n");
    scanf("%s", &choice);

    switch(choice) {
        case '1':
            addGrades(Nome_Materia, Nome_Turma, connection);
            break;
        case '2':
            system("cls");
            showTopAverages(Nome_Materia, Nome_Turma, connection);
            break;
        case '3':
            system("cls");
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
        default:
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
    }
}

void addGrades(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char tmp;
    char matricula[6];
    char p_nota[6], s_nota[6], t_nota[6], media[6];

    printf("Matricula: ");
    scanf("%s", &matricula);

    char query_matricula[131] = "SELECT Matricula FROM tbl_Alunos_Materias WHERE Matricula = ";
    strcat(query_matricula, matricula);
    strcat(query_matricula, " AND Nome_Materia = '");
    strcat(query_matricula, Nome_Materia);
    strcat(query_matricula, "'");

    if(mysql_query(connection, query_matricula)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    MYSQL_ROW row = mysql_fetch_row(result);

    if(!row) {
        mysql_free_result(result);
        printf("Matricula incorreta.\n");
        Sleep(2000);
        system("cls");
        showTable(Nome_Materia, Nome_Turma, connection);
    }

    printf("Primeira nota: ");
    scanf("%5s", &p_nota);
    scanf("%c", &tmp);
    printf("Segunda nota: ");
    scanf("%5s", &s_nota);
    scanf("%c", &tmp);
    printf("Terceira nota: ");
    scanf("%5s", &t_nota);
    scanf("%c", &tmp);

    char update[165] = "UPDATE tbl_Alunos_Materias SET Primeira_Nota = ";
    float n1, n2, n3, x;

    n1 = atof(p_nota);
    n2 = atof(s_nota);
    n3 = atof(t_nota);

    x = (n1 + n2 + n3) / 3.0;

    gcvt(x, 4, media);

    strcat(update, p_nota);
    strcat(update, ", Segunda_Nota = ");
    strcat(update, s_nota);
    strcat(update, ", Terceira_Nota = ");
    strcat(update, t_nota);
    strcat(update, ", Media = ");
    strcat(update, media);
    strcat(update, " WHERE Matricula = ");
    strcat(update, matricula);
    strcat(update, " AND Nome_Materia = '");
    strcat(update, Nome_Materia);
    strcat(update, "'");

    if(mysql_query(connection, update)) {
        exitWithError(connection);
    } else {
        mysql_free_result(result);
        printf("Notas modificadas com sucesso.");
        Sleep(2000);
        system("cls");
        showTable(Nome_Materia, Nome_Turma, connection);
    }
}

void showTopAverages(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char matricula[7];
    char query_medias[92] = "SELECT Matricula, Media FROM tbl_Alunos_Materias WHERE Nome_Materia = '";
    strcat(query_medias, Nome_Materia);
    strcat(query_medias, "' ORDER BY Media DESC");

    if(mysql_query(connection, query_medias)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    unsigned int fields = mysql_num_fields(result);

    MYSQL_ROW row;


    char medias[11][6];
    unsigned char count = 0;
    char query_matricula[101];

    printf("+-----+----------------------------------------+------+\n");
    printf("|Media|Nome                                    |Turma |\n");
    printf("+-----+----------------------------------------+------+\n");

    while(row = mysql_fetch_row(result)) {
        strcpy(query_matricula, "SELECT Matricula, Nome_Aluno FROM tbl_Alunos WHERE Matricula = ");
        for(unsigned char i = 0; i < fields; i++) {
            strcpy(matricula, row[0]);
        }
        strcat(query_matricula, matricula);
        strcat(query_matricula, " AND NOT Nome_Turma = '");
        strcat(query_matricula, Nome_Turma);
        strcat(query_matricula, "'");

        if(mysql_query(connection, query_matricula)) {
            exitWithError(connection);
        }

        MYSQL_RES *result_matricula = mysql_store_result(connection);

        checkResult(result_matricula, connection);

        MYSQL_ROW row_matricula;

        char query_nome[61] = "SELECT Nome_Aluno FROM tbl_Alunos WHERE Matricula = ";
        strcat(query_nome, matricula);

        if(mysql_query(connection, query_nome)) {
            exitWithError(connection);
        }

        MYSQL_RES *result_nome = mysql_store_result(connection);

        checkResult(result_nome, connection);
        MYSQL_ROW row_nome = mysql_fetch_row(result_nome);

        if(!(row_matricula = mysql_fetch_row(result_matricula)) && count < 10) {
            printf("|%s", row[1] ? row[1] : "  -  ");
            addTableSpaces(row, 5, 1);
            printf("|%s", row_nome[0] ? row_nome[0] : "  -  ");
            addTableSpaces(row_nome, 40, 0);
            printf("|%s|", Nome_Turma);
            printf("\n");
        }

        mysql_free_result(result_nome);
        mysql_free_result(result_matricula);
    }
    printf("+-----+----------------------------------------+------+\n");

    mysql_free_result(result);

    char choice;
    printf("1 - Voltar\n");
    scanf("%s", &choice);

    switch(choice) {
        case '1':
            system("cls");
            showTable(Nome_Materia, Nome_Turma, connection);
            break;
        default:
            system("cls");
            showTable(Nome_Materia, Nome_Turma, connection);
            break;
    }
}

void addStudent(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char tmp;
    char matricula[6];
    char nome[40];

    system("cls");
    printf("Adicionar Aluno\n");
    printf("Matricula: ");
    scanf("%5s", &matricula);
    scanf("%c", &tmp);

    char query_matricula[61] = "SELECT Matricula FROM tbl_Alunos WHERE Matricula = ";
    strcat(query_matricula, matricula);

    if(mysql_query(connection, query_matricula)) {
        exitWithError(connection);
    }

    MYSQL_RES *result_matricula = mysql_store_result(connection);

    checkResult(result_matricula, connection);

    MYSQL_ROW row_matricula = mysql_fetch_row(result_matricula);

    if(row_matricula) {
        printf("Essa matricula ja existe.\n");
        Sleep(2000);
        system("cls");
        addStudent(Nome_Materia, Nome_Turma, connection);
    }

    char insert_aluno[131] = "INSERT INTO tbl_Alunos(Matricula, Nome_Aluno, Nome_Turma) VALUES (";
    printf("Nome: ");
    scanf("%[^\n]%*c", nome);

    strcat(insert_aluno, matricula);
    strcat(insert_aluno, ", '");
    strcat(insert_aluno, nome);
    strcat(insert_aluno, "', '");
    strcat(insert_aluno, Nome_Turma);
    strcat(insert_aluno, "')");

    char choice;

    printf("1 - Confirmar\n");
    printf("2 - Sair\n");
    scanf("%c", &choice);

    switch(choice) {
        case '1':
            break;
        case '2':
            system("cls");
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
        default:
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
    }

    if(mysql_query(connection, insert_aluno)) {
        exitWithError(connection);
    }

    char query_materias[73] = "SELECT Nome_Materia FROM tbl_Turmas_Materias WHERE Nome_Turma = '";
    strcat(query_materias, Nome_Turma);
    strcat(query_materias, "'");

    if(mysql_query(connection, query_materias)) {
        exitWithError(connection);
    }

    MYSQL_RES *result = mysql_store_result(connection);

    checkResult(result, connection);

    unsigned int fields = mysql_num_fields(result);

    MYSQL_ROW row;

    char insert_aluno_materia[119];

    while(row = mysql_fetch_row(result)) {
        for(unsigned char i = 0; i < fields; i++) {
            strcpy(insert_aluno_materia, "INSERT INTO tbl_Alunos_Materias(Matricula, Nome_Materia) VALUES (");
            strcat(insert_aluno_materia, matricula);
            strcat(insert_aluno_materia, ", '");
            strcat(insert_aluno_materia, row[i]);
            strcat(insert_aluno_materia, "')");

            if(mysql_query(connection, insert_aluno_materia)) {
                exitWithError(connection);
            }
        }
    }

    mysql_free_result(result);
    mysql_free_result(result_matricula);

    printf("Aluno adicionado com sucesso.\n");
    Sleep(2000);
    showMainMenu(Nome_Materia, Nome_Turma, connection);
}

void removeStudent(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char tmp;
    char matricula[6];

    printf("Remover aluno\n");
    printf("Matricula: ");
    scanf("%s", &matricula);
    scanf("%c", &tmp);

    char query_matricula[73] = "SELECT Matricula, Nome_Aluno FROM tbl_Alunos WHERE Matricula = ";
    strcat(query_matricula, matricula);

    if(mysql_query(connection, query_matricula)) {
        exitWithError(connection);
    }

    MYSQL_RES *result_matricula = mysql_store_result(connection);

    checkResult(result_matricula, connection);

    MYSQL_ROW row_matricula = mysql_fetch_row(result_matricula);

    if(!row_matricula) {
        printf("Matricula incorreta.\n");
        Sleep(2000);
        system("cls");
        removeStudent(Nome_Materia, Nome_Turma, connection);
    }

    char choice;
    printf("Remover aluno '%s'?\n", row_matricula[1]);
    printf("1 - Confirmar\n");
    printf("2 - Sair\n");
    scanf("%c", &choice);

    switch(choice) {
        case '1':
            break;
        case '2':
            system("cls");
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
        default:
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
    }

    char delete_alunos_materias[61] = "DELETE FROM tbl_Alunos_Materias WHERE Matricula = ";
    strcat(delete_alunos_materias, matricula);

    if(mysql_query(connection, delete_alunos_materias)) {
        exitWithError(connection);
    }

    char delete_alunos[51] = "DELETE FROM tbl_Alunos WHERE Matricula = ";
    strcat(delete_alunos, matricula);

    if(mysql_query(connection, delete_alunos)) {
        exitWithError(connection);
    } else {
        printf("Aluno removido com sucesso.");
    }

    mysql_free_result(result_matricula);

    Sleep(2000);
    showMainMenu(Nome_Materia, Nome_Turma, connection);
}

void writeCSVFile(char Nome_Materia[40], char Nome_Turma[7], MYSQL *connection) {
    char file_name[52];
    strcat(file_name, Nome_Turma);
    strcat(file_name, Nome_Materia);
    strcat(file_name, ".csv");

    FILE *file;

    file = fopen(file_name, "w");

    fprintf(file, "Matr; Nome; Disciplina; Curso; Turma; 1N; 2N; 3N; Media");
    fprintf(file, "\n");

    char query_alunos_curso[139] = "SELECT A.Matricula, A.Nome_Aluno, T.Curso FROM tbl_Alunos AS A, tbl_Turmas AS T WHERE A.Nome_Turma = '";
    strcat(query_alunos_curso, Nome_Turma);
    strcat(query_alunos_curso, "' AND T.Nome_Turma = '");
    strcat(query_alunos_curso, Nome_Turma);
    strcat(query_alunos_curso, "'");

    if(mysql_query(connection, query_alunos_curso)) {
        exitWithError(connection);
    }

    MYSQL_RES *result_alunos_curso = mysql_store_result(connection);

    checkResult(result_alunos_curso, connection);

    MYSQL_ROW row_alunos_curso;

    while(row_alunos_curso = mysql_fetch_row(result_alunos_curso)) {
        for(unsigned char i = 0; i < 3; i++) {
            fprintf(file, "%s;", row_alunos_curso[i] ? row_alunos_curso[i] : "  -  ");
            i == 1 ? fprintf(file, "%s;", Nome_Materia) : NULL;
        }
        fprintf(file, "%s;", Nome_Turma);

        char matricula[6];
        strcpy(matricula, row_alunos_curso[0]);

        char query_notas[128] = "SELECT Primeira_Nota, Segunda_Nota, Terceira_Nota, Media FROM tbl_Alunos_Materias WHERE Matricula = ";
        strcat(query_notas, matricula);
        strcat(query_notas, " AND Nome_Materia = '");
        strcat(query_notas, Nome_Materia);
        strcat(query_notas, "'");

        if(mysql_query(connection, query_notas)) {
            exitWithError(connection);
        }

        MYSQL_RES *result_notas = mysql_store_result(connection);

        checkResult(result_notas, connection);

        MYSQL_ROW row_notas;

        while(row_notas = mysql_fetch_row(result_notas)) {
            for(unsigned char i = 0; i < 4; i++) {
                fprintf(file, "%s", row_notas[i] ? row_notas[i] : "  -  ");
                i == 3 ? fprintf(file, "") : fprintf(file, ";");
            }
        }

        fprintf(file, "\n");
    }

    fclose(file);

    char choice;
    printf("Dados salvos no arquivo '%s'\n", file_name);
    printf("1 - Voltar\n");
    scanf("%s", &choice);

    switch(choice) {
        case '1':
            system("cls");
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
        default:
            showMainMenu(Nome_Materia, Nome_Turma, connection);
            break;
    }
}






