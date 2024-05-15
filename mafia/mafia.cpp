#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

enum Role { MAFIA, CIVILIAN, DOCTOR, DETECTIVE };

class Player {
private:
    Role role;
    bool alive;
    bool protectedByDoctor;

public:
    Player(Role r) : role(r), alive(true), protectedByDoctor(false) {}

    Role getRole() const {
        return role;
    }

    bool isAlive() const {
        return alive;
    }

    void kill() {
        if (!protectedByDoctor) {
            alive = false;
        }
        else {
            std::cout << "Мафия не может убить игрока, потому что он защищен доктором.\n";
            protectedByDoctor = false; // защита снята после неудачной попытки убийства
        }
    }

    void revive() {
        alive = true;
    }

    void heal() {
        protectedByDoctor = true;
    }
};

class Game {
private:
    std::vector<Player> players;
    std::vector<int> mafiaKilledPlayers;
    std::vector<int> votedOutPlayers;
    int mafiaCount;
    int civilianCount;

public:
    Game(int playerCount) {
        mafiaCount = 1;
        civilianCount = playerCount - mafiaCount - 2; // Вычитаем 2 места для доктора и детектива

        for (int i = 0; i < playerCount; ++i) {
            if (i < mafiaCount)
                players.push_back(Player(MAFIA));
            else if (i < mafiaCount + 1)
                players.push_back(Player(DETECTIVE));
            else if (i < mafiaCount + 2)
                players.push_back(Player(DOCTOR));
            else
                players.push_back(Player(CIVILIAN));
        }

        std::shuffle(players.begin(), players.end(), std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count()));
    }

    void printRole(int playerIndex) {
        switch (players[playerIndex].getRole()) {
        case MAFIA:
            std::cout << "Вы мафия!\n";
            break;
        case CIVILIAN:
            std::cout << "Вы мирный житель!\n";
            break;
        case DOCTOR:
            std::cout << "Вы доктор!\n";
            break;
        case DETECTIVE:
            std::cout << "Вы детектив!\n";
            break;
        }
    }

    void mafiaTurn() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Мафия просыпается...\n";

        // Проверяем, является ли текущий игрок мафией
        if (players[0].getRole() == MAFIA) {
            // Логика выбора игрока, которого мафия убьет
            std::cout << "Выберите номер игрока для убийства: ";
            int target;
            std::cin >> target;
            target--; // Пользователь вводит номер с 1, а индексы начинаются с 0

            if (target < 0 || target >= players.size() || players[target].getRole() == MAFIA || !players[target].isAlive()) {
                std::cout << "Неверный выбор, повторите еще раз.\n";
                mafiaTurn(); // Повторяем ход мафии
                return;
            }

            std::cout << "Мафия выбирает игрока " << target + 1 << " для убийства.\n";
            players[target].kill();

            // Добавляем убитого игрока в вектор мафией убитых игроков
            mafiaKilledPlayers.push_back(target);

            // Удаляем выбранного игрока из вектора
            players.erase(players.begin() + target);
        }
        else {
            // Выбор игрока на рандом для других ролей
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, players.size() - 1);

            int target = dis(gen);
            while (!players[target].isAlive() || players[target].getRole() == MAFIA) {
                target = dis(gen);
            }
            std::cout << "Мафия выбирает игрока " << target + 1 << " для убийства.\n";

            // Добавляем убитого игрока в вектор мафией убитых игроков
            mafiaKilledPlayers.push_back(target);

            // Удаляем выбранного игрока из вектора
            players.erase(players.begin() + target);
        }

        std::cout << "Мафия засыпает...\n";
    }

    void doctorTurn() {
        std::cout << "Доктор просыпается...\n";
        // Логика доктора: он может выбрать игрока для лечения, чтобы предотвратить убийство в этот ход
        if (players[0].getRole() == DOCTOR) {
            std::cout << "Выберите номер игрока для лечения: ";
            int target;
            std::cin >> target;
            target--; // Пользователь вводит номер с 1, а индексы начинаются с 0

            if (target < 0 || target >= players.size() || !players[target].isAlive()) {
                std::cout << "Неверный выбор, повторите еще раз.\n";
                doctorTurn(); // Повторяем ход доктора
                return;
            }

            std::cout << "Доктор выбирает игрока " << target + 1 << " для лечения.\n";
            players[target].heal();
        }
        else {
            // Выбор игрока на рандом для других ролей
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, players.size() - 1);

            int target = dis(gen);
            while (!players[target].isAlive()) {
                target = dis(gen);
            }
            std::cout << "Доктор выбирает игрока " << target + 1 << " для лечения.\n";
            // Здесь можно добавить логику лечения игрока, например, устанавливаем флаг, что игрок защищен
            // protectedPlayers.push_back(target);

            // Удаляем выбранного игрока из вектора
            // players.erase(players.begin() + target);
        }

        std::cout << "Доктор засыпает...\n";
    }

    void detectiveTurn() {
        std::cout << "Детектив просыпается...\n";
        // Логика детектива: он может выбрать игрока для расследования, чтобы узнать его роль
        if (players[0].getRole() == DETECTIVE) {
            std::cout << "Выберите номер игрока для расследования: ";
            int target;
            std::cin >> target;
            target--; // Пользователь вводит номер с 1, а индексы начинаются с 0

            if (target < 0 || target >= players.size() || !players[target].isAlive()) {
                std::cout << "Неверный выбор, повторите еще раз.\n";
                detectiveTurn(); // Повторяем ход детектива
                return;
            }

            std::cout << "Детектив выбирает игрока " << target + 1 << " для расследования.\n";
            // Здесь можно добавить логику расследования, например, выводим роль игрока
            std::cout << "Роль игрока " << target + 1 << ": ";
            switch (players[target].getRole()) {
            case MAFIA:
                std::cout << "Мафия\n";
                break;
            case CIVILIAN:
                std::cout << "Мирный житель\n";
                break;
            }

            // Удаляем выбранного игрока из вектора
            // players.erase(players.begin() + target);
        }
        else {
            // Выбор игрока на рандом для других ролей
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, players.size() - 1);

            int target = dis(gen);
            while (!players[target].isAlive()) {
                target = dis(gen);
            }
            std::cout << "Детектив выбирает игрока " << target + 1 << " для расследования.\n";
            // Здесь можно добавить логику расследования, например, выводим роль игрока
            std::cout << "Роль игрока " << target + 1 << ": ";
            switch (players[target].getRole()) {
            case MAFIA:
                std::cout << "Мафия\n";
                break;
            case CIVILIAN:
                std::cout << "Мирный житель\n";
                break;
            }

            // Удаляем выбранного игрока из вектора
            // players.erase(players.begin() + target);
        }

        std::cout << "Детектив засыпает...\n";
    }

    void vote() {
        // Просто заглушка, можно добавить реальную логику голосования
        std::cout << "Голосование...\n";

        // Создаем вектор кандидатов для голосования
        std::vector<int> candidates;
        for (int i = 0; i < players.size(); ++i) {
            if (players[i].isAlive()) {
                bool isMafiaKilled = std::find(mafiaKilledPlayers.begin(), mafiaKilledPlayers.end(), i) != mafiaKilledPlayers.end();
                bool isVotedOut = std::find(votedOutPlayers.begin(), votedOutPlayers.end(), i) != votedOutPlayers.end();
                if (!isMafiaKilled && !isVotedOut) {
                    candidates.push_back(i);
                }
            }
        }

        // Проверка, есть ли кандидаты для голосования
        if (candidates.empty()) {
            std::cout << "Нет кандидатов для голосования. Игра завершается.\n";
            return;
        }

        int targetIndex; // Объявляем переменную targetIndex здесь

        // Если текущий игрок не мафия, то он может проголосовать
        std::cout << "Выберите номер игрока, за которого хотите проголосовать: ";
        int voteIndex;
        std::cin >> voteIndex;
        voteIndex--; // Пользователь вводит номер с 1, а индексы начинаются с 0

        if (voteIndex < 0 || voteIndex >= players.size() || !players[voteIndex].isAlive()) {
            std::cout << "Неверный выбор, повторите еще раз.\n";
            vote(); // Повторяем голосование
            return;
        }

        // Проверяем, не был ли игрок уже убит мафией в текущем или предыдущих голосованиях
        bool isMafiaKilled = std::find(mafiaKilledPlayers.begin(), mafiaKilledPlayers.end(), voteIndex) != mafiaKilledPlayers.end();
        if (isMafiaKilled) {
            std::cout << "Игрок " << voteIndex + 1 << " уже был убит мафией и не может быть выбран для голосования.\n";
            vote(); // Повторяем голосование
            return;
        }

        // Проверяем, не был ли игрок уже выбран на голосовании в текущем раунде
        bool alreadyVoted = std::find(votedOutPlayers.begin(), votedOutPlayers.end(), voteIndex) != votedOutPlayers.end();
        if (alreadyVoted) {
            std::cout << "Вы уже проголосовали за этого игрока. Выберите другого.\n";
            vote(); // Повторяем голосование
            return;
        }

        std::cout << "Игрок " << voteIndex + 1 << " выбран для голосования.\n";
        targetIndex = voteIndex; // Присваиваем значение voteIndex переменной targetIndex

        // Удаляем выбранного игрока из вектора участников голосования
        candidates.erase(std::remove(candidates.begin(), candidates.end(), targetIndex), candidates.end());
        // Добавляем выбранного игрока в вектор уже проголосовавших
        votedOutPlayers.push_back(targetIndex);
        // Убиваем выбранного игрока
        players[targetIndex].kill();
    }

    void play() {
        bool mafiaWon = false;
        bool civiliansWon = false;

        while (!mafiaWon && !civiliansWon) {
            doctorTurn();
            detectiveTurn();
            mafiaTurn();
            vote();

            int mafiaAlive = 0;
            int otherRolesAlive = 0;

            for (int i = 0; i < players.size(); ++i) {
                if (players[i].isAlive()) {
                    switch (players[i].getRole()) {
                    case MAFIA:
                        mafiaAlive++;
                        break;
                    case CIVILIAN:
                    case DOCTOR:
                    case DETECTIVE:
                        otherRolesAlive++;
                        break;
                    }
                }
            }

            // Проверяем условия победы
            if (mafiaAlive >= otherRolesAlive) {
                mafiaWon = true;
            }

            if (mafiaAlive == 0) {
                civiliansWon = true;
            }
        }

        if (mafiaWon) {
            std::cout << "Победила мафия!\n";
        }
        else {
            std::cout << "Победили мирные жители!\n";
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    int playerCount;

    while (true) {
        std::cout << "Введите количество игроков (от 5 до 7): ";
        std::cin >> playerCount;

        if (playerCount >= 5 && playerCount <= 7) {
            break;
        }

        std::cout << "Неправильное количество игроков! Попробуйте еще раз.\n";
    }

    Game game(playerCount);
    std::cout << "Ваша роль:\n";
    game.printRole(0);
    game.play();

    return 0;
}