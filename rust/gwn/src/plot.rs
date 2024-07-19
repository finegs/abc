
use plotpy::{generate3d, Plot, StrError, Surface};

fn main() -> Result<(), StrError> {
    let mut surface = Surface::new();
    surface
       .set_with_wireframe(true)
       .set_colormap_name("Pastel1")
       .set_with_colorbar(true)
       .set_colorbar_label("temperature")
       .set_wire_line_color("#1862ab")
       .set_wire_line_style(":")
       .set_wire_line_width(0.75);

    // draw surface
    let n = 9;
    let (x, y, z) = generate3d(-2.0, 2.0, -2.0, 2.0, n, n, |x, y| x * x + y * y);
    surface.draw(&x, &y, &z);

    // add surface to plot
    let mut plot = Plot::new();
    plot.add(&surface);

    // save figure
    plot.save("/tmp/plotpy/example_main.svg")?;
    Ok(())
}
