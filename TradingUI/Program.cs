// Console.WriteLine("Hello, World!");

#:package Aprillz.MewUI@0.9.0
using MewUI;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TradingUI;

class Program
{
    static TcpClient? _client;
    static NetworkStream? _stream;
    
    static async Task Main(string[] args)
    {
        // Connect to C++ matching engine
        try
        {
            _client = new TcpClient();
            await _client.ConnectAsync("127.0.0.1", 5555);
            _stream = _client.GetStream();
        }
        catch
        {
            // Will show connection status in UI
        }
        
        // Build the UI - NO XAML, pure C#
        var window = new Window()
            .Title("Trading Client - C++ Matching Engine")
            .Size(500, 450)
            .CenterScreen()
            .Content(
                new Border()
                .Padding(20)
                .Child(
                    new StackPanel().Spacing(15).Children(
                        
                        // Header
                        new Label()
                            .Text("Place Order")
                            .FontSize(18)
                            .FontWeight(FontWeight.Bold)
                            .HorizontalAlignment(HorizontalAlignment.Center),
                        
                        // Connection status indicator
                        new StackPanel().Orientation(Orientation.Horizontal).Spacing(10).Children(
                            new Border()
                                .Width(12).Height(12)
                                .CornerRadius(6)
                                .Background(_client?.Connected == true ? "#4CAF50" : "#F44336"),
                            new Label()
                                .Text(_client?.Connected == true ? "Connected to Engine" : "Disconnected")
                                .Foreground(_client?.Connected == true ? "#4CAF50" : "#F44336")
                        ),
                        
                        // Divider
                        new Border().Height(1).Background("#E0E0E0").Margin(0, 5, 0, 5),
                        
                        // Order form
                        new Grid().Columns("Auto,*").Rows("Auto,Auto,Auto,Auto").ColumnSpacing(10).RowSpacing(15).Children(
                            
                            // Row 0 - Symbol
                            new Label().Text("Symbol:").Row(0).Column(0).VerticalAlignment(VerticalAlignment.Center),
                            new ComboBox()
                                .Row(0).Column(1)
                                .Width(200)
                                .Placeholder("Select or type symbol")
                                .Items("AAPL", "MSFT", "GOOGL", "AMZN", "TSLA", "META")
                                .SelectedIndex(0),
                            
                            // Row 1 - Side
                            new Label().Text("Side:").Row(1).Column(0).VerticalAlignment(VerticalAlignment.Center),
                            new StackPanel().Row(1).Column(1).Orientation(Orientation.Horizontal).Spacing(20).Children(
                                new RadioButton().Content("BUY").GroupName("Side").IsChecked(true),
                                new RadioButton().Content("SELL").GroupName("Side")
                            ),
                            
                            // Row 2 - Quantity
                            new Label().Text("Quantity:").Row(2).Column(0).VerticalAlignment(VerticalAlignment.Center),
                            new NumericUpDown()
                                .Row(2).Column(1)
                                .Width(200)
                                .Minimum(1)
                                .Maximum(10000)
                                .Value(100),
                            
                            // Row 3 - Price
                            new Label().Text("Price ($):").Row(3).Column(0).VerticalAlignment(VerticalAlignment.Center),
                            new TextBox()
                                .Row(3).Column(1)
                                .Width(200)
                                .Placeholder("150.25")
                                .Text("150.25")
                        ),
                        
                        // Order preview
                        new Border()
                            .Background("#F5F5F5")
                            .CornerRadius(5)
                            .Padding(15)
                            .Margin(0, 10, 0, 0)
                            .Child(
                                new StackPanel().Spacing(5).Children(
                                    new Label().Text("Order Preview").FontWeight(FontWeight.Bold),
                                    new Label().Text("AAPL | BUY | 100 shares @ $150.25")
                                        .Foreground("#2196F3")
                                        .FontSize(14)
                                )
                            ),
                        
                        // Buttons
                        new StackPanel().Orientation(Orientation.Horizontal).Spacing(10).HorizontalAlignment(HorizontalAlignment.Right).Children(
                            new Button()
                                .Content("Cancel")
                                .Width(100)
                                .Padding(10, 8)
                                .Background("#F44336")
                                .Foreground("White")
                                .BorderThickness(0)
                                .CornerRadius(3)
                                .OnClick(async (s, e) => await Application.Current.CloseWindowAsync(window)),
                            
                            new Button()
                                .Content("Place Order")
                                .Width(120)
                                .Padding(10, 8)
                                .Background(_client?.Connected == true ? "#4CAF50" : "#9E9E9E")
                                .Foreground("White")
                                .BorderThickness(0)
                                .CornerRadius(3)
                                .IsEnabled(_client?.Connected == true)
                                .OnClick(async (s, e) => await PlaceOrder())
                        ),
                        
                        // Status message area
                        new Border()
                            .Margin(0, 10, 0, 0)
                            .Child(
                                new Label()
                                    .Name("StatusLabel")
                                    .Text("Ready")
                                    .Foreground("#757575")
                                    .FontSize(12)
                            )
                    )
                )
            );
        
        // Run the application
        await Application.RunAsync(window);
    }

    static async Task PlaceOrder()
    {
        if (_stream == null || !_client?.Connected == true)
        {
            UpdateStatus("Not connected to engine", "#F44336");
            return;
        }
        
        try
        {
            UpdateStatus("Sending order...", "#2196F3");
            
            // Format: "SYMBOL|SIDE|QUANTITY|PRICE"
            // Example: "AAPL|BUY|100|150.25"
            string order = $"AAPL|BUY|100|150.25";
            byte[] data = Encoding.UTF8.GetBytes(order);
            
            await _stream.WriteAsync(data, 0, data.Length);
            
            // Wait for response (optional)
            byte[] buffer = new byte[1024];
            int bytesRead = await _stream.ReadAsync(buffer, 0, buffer.Length);
            string response = Encoding.UTF8.GetString(buffer, 0, bytesRead);
            
            UpdateStatus($"Order placed: {response}", "#4CAF50");
        }
        catch (Exception ex)
        {
            UpdateStatus($"Error: {ex.Message}", "#F44336");
        }
    }
    
    static void UpdateStatus(string message, string color)
    {
        // Find the status label and update it
        var statusLabel = Application.Current.Windows
            .SelectMany(w => w.FindVisualChildren<Label>())
            .FirstOrDefault(l => l.Name == "StatusLabel");
            
        if (statusLabel != null)
        {
            statusLabel.Text = message;
            statusLabel.Foreground = color;
        }
    }
}