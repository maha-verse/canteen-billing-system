#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

struct MenuItem {
    int id;
    std::string name;
    double price;
};
struct OrderItem {
    int id;
    std::string name;
    int quantity;
    double unitPrice, total;
};
struct Order {
    int orderId;
    std::vector<OrderItem> items;
    double subtotal, discount, total;
};

void saveMenu(const std::vector<MenuItem>& menu) {
    std::ofstream file("menu.txt");
    if (!file.is_open()) return;
    for (const auto& item : menu) file << item.id << "," << item.name << "," << item.price << "\n";
}

void loadMenu(std::vector<MenuItem>& menu) {
    std::ifstream file("menu.txt");
    menu.clear();
    std::string line, idStr, name, priceStr;
    while (file && std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        if (std::getline(ss, idStr, ',') && std::getline(ss, name, ',') && std::getline(ss, priceStr, ','))
            menu.push_back({std::stoi(idStr), name, std::stod(priceStr)});
    }
    if (menu.empty()) {
        menu = {{1, "Idli", 20.0}, {2, "Dosa", 40.0}, {3, "Poori", 35.0},
                {4, "Pongal", 35.0}, {5, "Sandwich", 50.0}, {6, "Burger", 60.0},
                {7, "Veg Fried Rice", 70.0}, {8, "Samosa", 15.0},
                {9, "Tea", 10.0}, {10, "Coffee", 20.0}};
        saveMenu(menu);
    }
}

void saveOrder(const Order& order) {
    std::ofstream file("orders.txt", std::ios::app);
    if (!file.is_open()) return;
    file << "ORDER " << order.orderId << " " << order.subtotal << " " << order.discount << " " << order.total << "\n";
    for (const auto& it : order.items)
        file << "ITEM " << it.id << " " << it.quantity << " " << it.unitPrice << " " << it.total << " " << it.name << "\n";
    file << "END\n";
}

void loadOrders(std::vector<Order>& orders) {
    std::ifstream file("orders.txt");
    if (!file.is_open()) return;
    std::string tag, subTag;
    while (file >> tag) {
        if (tag == "ORDER") {
            Order ord;
            file >> ord.orderId >> ord.subtotal >> ord.discount >> ord.total;
            while (file >> subTag && subTag == "ITEM") {
                OrderItem item;
                file >> item.id >> item.quantity >> item.unitPrice >> item.total;
                file.ignore();
                std::getline(file, item.name);
                ord.items.push_back(item);
            }
            orders.push_back(ord);
        }
    }
}

void displayMenu(const std::vector<MenuItem>& menu) {
    std::cout << "\n================== FOOD MENU ==================\n"
              << std::left << std::setw(6) << "ID" << std::setw(22) << "Item Name"
              << std::right << std::setw(12) << "Price (Rs)\n"
              << "------------------------------------------------\n";
    for (const auto& it : menu) {
        std::cout << std::left << std::setw(6) << it.id << std::setw(22) << it.name
                  << std::right << std::setw(12) << std::fixed << std::setprecision(2) << it.price << "\n";
    }
    std::cout << "================================================\n";
}

void calculateBill(Order& order) {
    order.subtotal = 0.0;
    for (auto& item : order.items) {
        item.total = item.quantity * item.unitPrice;
        order.subtotal += item.total;
    }
    order.discount = (order.subtotal >= 500.0) ? (order.subtotal * 0.10) : 0.0;
    order.total = order.subtotal - order.discount;
}

void printReceipt(const Order& order) {
    std::cout << "\n================ CASH RECEIPT ================\n"
              << "Order Number: #" << order.orderId << "\n"
              << "------------------------------------------------\n"
              << std::left << std::setw(18) << "Item" << std::setw(6) << "Qty"
              << std::setw(12) << "Unit Price" << std::right << std::setw(10) << "Total\n"
              << "------------------------------------------------\n";
    for (const auto& it : order.items) {
        std::cout << std::left << std::setw(18) << it.name << std::setw(6) << it.quantity
                  << "Rs." << std::setw(9) << std::fixed << std::setprecision(2) << it.unitPrice
                  << "Rs." << std::right << std::setw(7) << it.total << "\n";
    }
    std::cout << "------------------------------------------------\n"
              << std::left << std::setw(36) << "Subtotal:" << "Rs."
              << std::right << std::setw(8) << std::fixed << std::setprecision(2) << order.subtotal << "\n"
              << std::left << std::setw(36) << "Discount (10% if >= Rs.500):" << "Rs."
              << std::right << std::setw(8) << order.discount << "\n"
              << "------------------------------------------------\n"
              << std::left << std::setw(36) << "Final Amount Payable:" << "Rs."
              << std::right << std::setw(8) << order.total << "\n"
              << "================================================\n"
              << "           Thank you! Visit again!\n"
              << "================================================\n";
}

void createOrder(const std::vector<MenuItem>& menu, std::vector<Order>& orders) {
    displayMenu(menu);
    Order newOrder;
    newOrder.orderId = orders.empty() ? 1001 : orders.back().orderId + 1;
    std::cout << "\n--- Creating Order #" << newOrder.orderId << " ---\n";
    while (true) {
        std::cout << "Enter Item ID (0 to complete order): ";
        int itemId;
        if (!(std::cin >> itemId)) {
            std::cin.clear(); std::cin.ignore(1000, '\n');
            std::cout << "Invalid input! Please enter a numeric Item ID.\n";
            continue;
        }
        if (itemId == 0) break;

        const MenuItem* sel = nullptr;
        for (const auto& item : menu) if (item.id == itemId) { sel = &item; break; }
        if (!sel) { std::cout << "Item ID not found in menu. Try again.\n"; continue; }

        std::cout << "Enter quantity for " << sel->name << ": ";
        int qty;
        if (!(std::cin >> qty) || qty <= 0) {
            std::cin.clear(); std::cin.ignore(1000, '\n');
            std::cout << "Invalid quantity! Quantity must be greater than 0.\n";
            continue;
        }

        bool found = false;
        for (auto& it : newOrder.items) {
            if (it.id == itemId) { it.quantity += qty; found = true; break; }
        }
        if (!found) newOrder.items.push_back({sel->id, sel->name, qty, sel->price, 0.0});
        std::cout << "Added " << qty << "x " << sel->name << " to order.\n";
    }

    if (newOrder.items.empty()) {
        std::cout << "Order cancelled. No items were selected.\n";
        return;
    }
    calculateBill(newOrder);
    printReceipt(newOrder);
    saveOrder(newOrder);
    orders.push_back(newOrder);
    std::cout << "Order #" << newOrder.orderId << " recorded successfully!\n";
}

void viewOrderHistory(const std::vector<Order>& orders) {
    if (orders.empty()) { std::cout << "\nNo previous orders found.\n"; return; }
    std::cout << "\n================ PREVIOUS ORDERS ================\n";
    for (const auto& ord : orders) {
        std::cout << "Order #" << ord.orderId << " | Subtotal: Rs." << std::fixed << std::setprecision(2)
                  << ord.subtotal << " | Discount: Rs." << ord.discount << " | Total: Rs." << ord.total << "\n";
        for (const auto& it : ord.items) {
            std::cout << "   - " << it.name << " (x" << it.quantity << ") @ Rs." << it.unitPrice << " = Rs." << it.total << "\n";
        }
        std::cout << "------------------------------------------------\n";
    }
}

void showSalesSummary(const std::vector<Order>& orders) {
    std::cout << "\n================ SALES SUMMARY ================\n";
    int totalCount = static_cast<int>(orders.size());
    double totalRevenue = 0.0, totalDiscounts = 0.0;
    for (const auto& ord : orders) {
        totalRevenue += ord.total;
        totalDiscounts += ord.discount;
    }
    std::cout << "Total Orders Completed : " << totalCount << "\n"
              << "Total Revenue Collected: Rs." << std::fixed << std::setprecision(2) << totalRevenue << "\n"
              << "Total Discounts Given  : Rs." << totalDiscounts << "\n";
    if (totalCount > 0) std::cout << "Average Order Value    : Rs." << (totalRevenue / totalCount) << "\n";
    std::cout << "===============================================\n";
}

void updateFoodMenu(std::vector<MenuItem>& menu) {
    displayMenu(menu);
    std::cout << "\n1. Update existing item price\n2. Add a new food item\nEnter choice (1-2): ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear(); std::cin.ignore(1000, '\n');
        std::cout << "Invalid input.\n";
        return;
    }

    if (choice == 1) {
        std::cout << "Enter Item ID to update: ";
        int itemId;
        if (!(std::cin >> itemId)) { std::cin.clear(); std::cin.ignore(1000, '\n'); return; }
        for (auto& item : menu) {
            if (item.id == itemId) {
                std::cout << "Enter new price for " << item.name << ": ";
                double newPrice;
                if (std::cin >> newPrice && newPrice > 0) {
                    item.price = newPrice;
                    saveMenu(menu);
                    std::cout << "Price updated successfully!\n";
                } else { std::cin.clear(); std::cin.ignore(1000, '\n'); std::cout << "Invalid price!\n"; }
                return;
            }
        }
        std::cout << "Item ID not found!\n";
    } else if (choice == 2) {
        std::cout << "Enter new item name: ";
        std::cin.ignore();
        std::string name;
        std::getline(std::cin, name);
        std::cout << "Enter price: ";
        double price;
        if (!name.empty() && (std::cin >> price) && price > 0) {
            int newId = menu.empty() ? 1 : menu.back().id + 1;
            menu.push_back({newId, name, price});
            saveMenu(menu);
            std::cout << "Item '" << name << "' added with ID " << newId << ".\n";
        } else { std::cin.clear(); std::cin.ignore(1000, '\n'); std::cout << "Invalid item details.\n"; }
    } else {
        std::cout << "Invalid choice.\n";
    }
}

int main() {
    std::vector<MenuItem> menu;
    std::vector<Order> orders;
    loadMenu(menu);
    loadOrders(orders);

    int choice = 0;
    while (choice != 6) {
        std::cout << "\n========================================\n"
                  << "        CANTEEN BILLING SYSTEM          \n"
                  << "========================================\n"
                  << "1. View food menu\n2. Create a new order\n3. View previous orders\n"
                  << "4. View sales summary\n5. Add or update food prices\n6. Exit\n"
                  << "Enter your choice (1-6): ";

        if (!(std::cin >> choice)) {
            std::cin.clear(); std::cin.ignore(1000, '\n');
            std::cout << "Invalid input! Please enter a number between 1 and 6.\n";
            continue;
        }

        switch (choice) {
            case 1: displayMenu(menu); break;
            case 2: createOrder(menu, orders); break;
            case 3: viewOrderHistory(orders); break;
            case 4: showSalesSummary(orders); break;
            case 5: updateFoodMenu(menu); break;
            case 6: std::cout << "\nThank you for using Canteen Billing System. Goodbye!\n"; break;
            default: std::cout << "Invalid option! Please select from 1 to 6.\n"; break;
        }
    }
    return 0;
}
