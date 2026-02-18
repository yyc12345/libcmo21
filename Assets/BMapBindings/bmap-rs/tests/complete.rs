use bmap_rs::bmap_wrapper as bmap;

#[test]
fn test_complete() {
    assert!(bmap::BMap::is_available());
    bmap::BMap::with_bmap(|b| {
        let r = b.create_file_reader("", "", "", &[""]);
    });
}
