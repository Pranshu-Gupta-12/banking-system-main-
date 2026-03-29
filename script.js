// Banking System - Common JavaScript Functions

console.log('Banking System Loaded');

// Initialize local storage if needed
function initializeStorage() {
    if (!localStorage.getItem('accounts')) {
        localStorage.setItem('accounts', JSON.stringify([]));
    }
}

// Add some sample accounts for testing (optional)
function addSampleAccounts() {
    const existing = JSON.parse(localStorage.getItem('accounts')) || [];
    if (existing.length === 0) {
        const samples = [
            {
                accountNumber: 'ACC001',
                ownerName: 'John Doe',
                accountType: 'savings',
                balance: 5000,
                transactions: [
                    {
                        type: 'OPEN',
                        amount: 5000,
                        balance_after: 5000,
                        date: new Date().toISOString()
                    }
                ]
            },
            {
                accountNumber: 'ACC002',
                ownerName: 'Jane Smith',
                accountType: 'checking',
                balance: 3500,
                transactions: [
                    {
                        type: 'OPEN',
                        amount: 3500,
                        balance_after: 3500,
                        date: new Date().toISOString()
                    }
                ]
            }
        ];
        localStorage.setItem('accounts', JSON.stringify(samples));
    }
}

// Format currency
function formatCurrency(amount) {
    return '$' + parseFloat(amount).toFixed(2);
}

// Get all accounts
function getAllAccounts() {
    return JSON.parse(localStorage.getItem('accounts')) || [];
}

// Get account by number
function getAccount(accountNumber) {
    const accounts = getAllAccounts();
    return accounts.find(acc => acc.accountNumber === accountNumber);
}

// Update account
function updateAccount(accountNumber, updates) {
    const accounts = getAllAccounts();
    const index = accounts.findIndex(acc => acc.accountNumber === accountNumber);
    if (index !== -1) {
        accounts[index] = { ...accounts[index], ...updates };
        localStorage.setItem('accounts', JSON.stringify(accounts));
        return true;
    }
    return false;
}

// Initialize on page load
window.addEventListener('DOMContentLoaded', function() {
    initializeStorage();
    // Uncomment to add sample accounts automatically
    // addSampleAccounts();
});

// Prevent form resubmission on page reload
if (window.history.replaceState) {
    window.history.replaceState(null, null, window.location.href);
}
